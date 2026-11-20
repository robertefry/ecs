
#ifndef ECS_HH
#define ECS_HH

#include "ecs/sparset.hh"

#include <cstddef>
#include <atomic>
#include <vector>
#include <functional>

namespace ecs {

    using entity_t = size_t;

    namespace impl {

        template <typename Component>
        struct storage {

            ecs::sparset<Component> data {};

            [[nodiscard]] static auto instance() -> storage& {
                static storage s_storage;
                return s_storage;
            }

        }; // struct storage

    } // namespace impl

    /**
     * Make a new entity
     */
    [[nodiscard]] inline auto entity() -> entity_t {
        static std::atomic_size_t s_entity_id_next {0};
        return s_entity_id_next.fetch_add(1, std::memory_order_acq_rel);
    }

    /**
     * Determine if an entity has a component.
     */
    template <typename Component>
    [[nodiscard]] auto has(entity_t eid) -> bool {
        return impl::storage<Component>::instance().data.contains(eid);
    }

    /**
     * Get a reference to the component of an entity.
     * UB if the entity does not have the component.
     */
    template <typename Component>
    [[nodiscard]] auto get(entity_t eid) -> Component& {
        return impl::storage<Component>::instance().data[eid];
    }

    /**
     * Access a contiguous span of all components.
     */
    template <typename Component>
    [[nodiscard]] auto all() -> std::span<Component> {
        return impl::storage<Component>::instance().data.span();
    }

    /**
     * Access a contiguous span of all entities with a component.
     */
    template <typename Component>
    [[nodiscard]] auto with() -> std::span<entity_t const> {
        return impl::storage<Component>::instance().data.keys();
    }

    /**
     * Count the number of registered components of this type.
     */
    template <typename Component>
    [[nodiscard]] auto count() -> size_t {
        return impl::storage<Component>::instance().data.size();
    }

    /**
     * Construct a component in-place for the given entity.
     */
    template <typename Component, typename... Args>
    auto emplace(entity_t eid, Args&&... args) -> Component&
    {
        auto& store = impl::storage<Component>::instance();
        auto [component, emplaced] = store.data.emplace(
            eid, std::forward<Args>(args)...
        );
        return component;
    }

    /**
     * Insert a component to the given entity.
     */
    template <typename Component>
    auto insert(entity_t eid, Component&& component) -> Component& {
        using C = std::remove_cvref_t<Component>;
        return ecs::emplace<C>(eid, std::forward<Component>(component));
    }

    /**
     * Remove a component from the given entity.
     */
    template <typename Component>
    void remove(entity_t eid) {
        auto const& store = impl::storage<Component>::instance();
        store.data.erase(eid);
    }

    /**
     * Execute a function for each entity with a set of components.
     */
    template <typename... Cs, typename F>
    void for_each(F&& func)
    {
        size_t const min_size = std::min({ecs::count<Cs>()...});
        auto const drivers = [&]<typename D>() -> bool
        {
            if (ecs::count<D>() != min_size) { return false; }
            for (entity_t eid : ecs::with<D>()) {
                if ((not ecs::has<Cs>(eid) || ...)) { continue; }
                func(eid, ecs::get<Cs>(eid)...);
            }
            return true;
        };
        (drivers.template operator()<Cs>() || ...);
    }

} // namespace ecs

#endif /* ECS_HH */
