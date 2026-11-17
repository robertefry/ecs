
#include "ecs/ecs.hh"

#include <atomic>

namespace {

    std::atomic_size_t s_entity_id_next = 0;

} // namespace

auto ecs::make_entity() -> entity {
    return s_entity_id_next.fetch_add(1, std::memory_order_acq_rel);
}
