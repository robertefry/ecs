
#ifndef ECS_WORLD_HH
#define ECS_WORLD_HH

#include <cstddef>
#include <unordered_map>
#include <atomic>

namespace ecs {

    using entity = size_t;

    [[nodiscard]] auto make_entity() -> entity;

} // namespace ecs

#endif /* ECS_WORLD_HH */
