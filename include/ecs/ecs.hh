
#ifndef ECS_HH
#define ECS_HH

#include "ecs/sparset.hh"

#include <cstddef>
#include <cassert>
#include <utility>
#include <concepts>
#include <span>
#include <tuple>
#include <forward_list>
#include <atomic>
#include <algorithm>
#include <type_traits>

namespace ecs {

    using entity_t = size_t;

    /**
     * Make a new entity
     */
    [[nodiscard]] inline auto entity() -> entity_t {
        static std::atomic_size_t s_entity_id_next {0};
        return s_entity_id_next.fetch_add(1, std::memory_order_acq_rel);
    }

    namespace impl {

        struct storage_base {

            virtual void clear() = 0;

            storage_base(storage_base const&) = delete;
            storage_base& operator=(storage_base const&) = delete;

            static auto registry() -> auto& {
                static std::forward_list<storage_base*> s_registry;
                return s_registry;
            }

            virtual ~storage_base() {
                registry().remove(this);
            }

            storage_base() {
                registry().push_front(this);
            }

        };

        template <typename Component>
        struct storage final : public storage_base {

            ecs::sparset<Component> data {};

            void clear() override { data.clear(); }

            [[nodiscard]] static auto instance() -> storage& {
                static storage s_storage;
                return s_storage;
            }

        }; // struct storage

    } // namespace impl

    // Lookup

    /**
     * Determine whether an entity has each component.
     */
    template <typename... Cs>
    [[nodiscard]] auto has(entity_t eid) -> bool {
        return (impl::storage<Cs>::instance().data.contains(eid) and ...);
    }

    /**
     * Count the number of registered components of this type.
     */
    template <typename Component>
    [[nodiscard]] auto count() -> size_t {
        return impl::storage<Component>::instance().data.size();
    }

    // Access

    /**
     * Get a reference to the component of an entity.
     * @note Behaviour is undefined if the entity does not have the component.
     */
    template <typename Component>
    [[nodiscard]] auto get(entity_t eid) -> Component& {
        assert(ecs::has<Component>(eid) && "Cannot get a component that does not exist");
        return impl::storage<Component>::instance().data[eid];
    }

    /**
     * Access the contiguous span of all components.
     */
    template <typename Component>
    [[nodiscard]] auto span() -> std::span<Component> {
        return impl::storage<Component>::instance().data.span();
    }

    /**
     * Access the contiguous span of all entities with a component.
     */
    template <typename Component>
    [[nodiscard]] auto with() -> std::span<entity_t const> {
        return impl::storage<Component>::instance().data.keys();
    }

    /**
     * Execute a function for each entity with a set of components.
     */
    template <typename... Cs, typename F>
    void for_each(F&& func)
    {
        size_t const min_size = std::min({ecs::count<Cs>()...});
        auto const drivers = [&]<typename D>() -> bool {
            if (ecs::count<D>() != min_size) { return false; }
            for (entity_t eid : ecs::with<D>()) {
                if (not ecs::has<Cs...>(eid)) { continue; }
                func(eid, ecs::get<Cs>(eid)...);
            }
            return true;
        };
        (drivers.template operator()<Cs>() || ...);
    }

    // Modifiers

    /**
     * Construct a component in-place for the given entity.
     * @note No construction will happen if the entity already has the component.
     * @returns A pair consisting of a reference to the component and whether construction took place.
     */
    template <typename Component, typename... Args>
    auto emplace(entity_t eid, Args&&... args) -> std::pair<Component&,bool>
    {
        auto& store = impl::storage<Component>::instance();
        auto pair = store.data.emplace(
            eid, std::forward<Args>(args)...
        );
        return pair;
    }

    /**
     * Insert a component to the given entity.
     * @note THe component will not be inserted if the entity already has the component.
     * @returns A pair consisting of a reference to the component and whether insertion took place.
     */
    template <typename Component>
    auto insert(entity_t eid, Component&& component) -> std::pair<Component&,bool> {
        using C = std::remove_cvref_t<Component>;
        return ecs::emplace<C>(eid, std::forward<Component>(component));
    }

    /**
     * Remove a component from the given entity.
     * @note Behaviour is undefined if the entity does not have the component.
     */
    template <typename Component>
    void remove(entity_t eid) {
        assert(ecs::has<Component>(eid) && "Cannot remove a component that does not exist");
        auto& store = impl::storage<Component>::instance();
        store.data.erase(eid);
    }

    /**
     * Remove components from an entity and return them by value.
     * @note Behaviour is undefined if the entity does not have every component.
     */
    template <typename Component>
    [[nodiscard]] auto take(entity_t eid) -> Component {
        assert(ecs::has<Component>(eid) && "Cannot take a component that does not exist");
        auto component = std::move(ecs::get<Component>(eid));
        ecs::remove<Component>(eid);
        return component;
    }

    /**
     * Erase all components of this type.
     */
    template <typename Component>
    void erase() {
        impl::storage<Component>::instance().data.clear();
    }

    /**
     * Clear everything from the ecs.
     */
    void clear() {
        for (auto* store : impl::storage_base::registry()) { store->clear(); }
    }

} // namespace ecs

#endif /* ECS_HH */
