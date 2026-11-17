
#ifndef ECS_HH
#define ECS_HH

#include <cstddef>
#include <unordered_map>
#include <atomic>

namespace ecs {

    using entity = size_t;

    namespace impl {

        template <typename component>
        class storage {
            std::unordered_map<ecs::entity,component> data_ {};
        public:
            [[nodiscard]] static auto instance() -> storage& {
                static storage s_storage;
                return s_storage;
            }
            [[nodiscard]] static auto data() -> auto& {
                return instance().data_;
            }
        };

    } // namespace impl

    [[nodiscard]] auto make_entity() -> entity {
        static std::atomic_size_t s_entity_id_next {0};
        return s_entity_id_next.fetch_add(1, std::memory_order_acq_rel);
    }

    template <typename component, typename... Args>
    auto add_component(entity eid, Args&&... args) -> component& {
        auto [it, _] = impl::storage<component>::data().emplace(
            eid, std::forward<Args>(args)...
        );
        return it->second;
    }

    template <typename component>
    [[nodiscard]] auto get_component(entity eid) -> component* {
        auto iter = impl::storage<component>::data().find(eid);
        if (iter == impl::storage<component>::data().end()) {
            return nullptr;
        }
        return &iter->second;
    }

    template <typename component>
    [[nodiscard]] auto has_component(entity eid) -> bool {
        return get_component<component>(eid) != nullptr;
    }

} // namespace ecs

#endif /* ECS_HH */
