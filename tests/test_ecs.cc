
#include <ecs/ecs.hh>

#include <catch2/catch_all.hpp>

TEST_CASE("get_component", "[component]")
{
    struct Text {
        std::string data;
        explicit Text(std::string data) : data{std::move(data)} {}
    };

    auto const entity = ecs::make_entity();
    ecs::add_component<Text>(entity, "Hello, World!");

    auto text = ecs::get_component<Text>(entity);
    REQUIRE(text != nullptr);
    REQUIRE(text->data == "Hello, World!");
}
