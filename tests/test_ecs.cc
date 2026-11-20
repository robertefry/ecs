
#include <ecs/ecs.hh>

#include <catch2/catch_all.hpp>
#include <ranges>
#include <format>

TEST_CASE("get_component", "[component]")
{
    struct Text {
        std::string data;
        explicit Text(std::string data) : data{std::move(data)} {}
    };

    auto const entity = ecs::entity();
    ecs::emplace<Text>(entity, "Hello, World!");

    auto& text = ecs::get<Text>(entity);
    REQUIRE(text.data == "Hello, World!");
}

TEST_CASE("bench: get_component", "[benchmark][component]")
{
    static constexpr size_t num_entities = 100'000;

    for (size_t i = 0; i < num_entities; ++i) {
        auto eid = ecs::entity();
        ecs::insert(eid, i);
    }

    volatile size_t sum = 0;

    BENCHMARK(std::format("num_entities = {}", num_entities))
    {
        sum = 0;
        ecs::for_each<size_t>([&](ecs::entity_t eid, size_t value) {
            sum += value;
        });
    };

    constexpr size_t target = num_entities * (num_entities - 1) >> 1;
    REQUIRE(sum == target);
}
