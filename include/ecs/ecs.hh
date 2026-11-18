
#ifndef ECS_HH
#define ECS_HH

#include "ecs/sparset.hh"

#include <cstddef>
#include <atomic>

namespace ecs {

    using entity = size_t;

    namespace impl {

        template <typename Component>
        class storage {
            ecs::sparset<Component> data_ {};
        public:
            [[nodiscard]] static auto instance() -> storage& {
                static storage s_storage;
                return s_storage;
            }
            [[nodiscard]] static auto sparset() -> auto& {
                return instance().data_;
            }
        };

    } // namespace impl

    [[nodiscard]] inline auto make_entity() -> entity {
        static std::atomic_size_t s_entity_id_next {0};
        return s_entity_id_next.fetch_add(1, std::memory_order_acq_rel);
    }

    template <typename Component, typename... Args>
    auto add_component(entity eid, Args&&... args) -> Component& {
        auto [component, _] = impl::storage<Component>::sparset().emplace(
            eid, std::forward<Args>(args)...
        );
        return component;
    }

    template <typename Component>
    [[nodiscard]] auto has_component(entity eid) -> bool {
        return impl::storage<Component>::sparset().contains(eid);
    }

    template <typename Component>
    [[nodiscard]] auto get_component(entity eid) -> Component* {
        return (not has_component<Component>(eid)) ? nullptr
            : &impl::storage<Component>::sparset()[eid];
    }

    template <typename Component>
    [[nodiscard]] auto components() -> std::span<Component> {
        return impl::storage<Component>::sparset().span();
    }

} // namespace ecs

#endif /* ECS_HH */
