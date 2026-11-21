
#include <ecs/ecs.hh>

#include <catch2/catch_all.hpp>
#include <set>
#include <ranges>
#include <format>
#include <iostream>

struct ecs_clear : Catch::EventListenerBase {
    using EventListenerBase::EventListenerBase;
    void testCaseStarting(Catch::TestCaseInfo const&) override { ecs::clear(); }
};
CATCH_REGISTER_LISTENER(ecs_clear);

struct Text {
    std::string data;
    explicit Text(std::string data) : data{std::move(data)} {}
    Text(Text&&) noexcept = default;
    Text& operator=(Text&&) noexcept = default;
    friend auto operator<=>(Text const&, Text const&) = default;
};

struct Color {
    float r,g,b;
    friend auto operator<=>(Color const&, Color const&) = default;
};

TEST_CASE("ecs lookup (has, count)")
{
    auto const e1 = ecs::entity();
    auto const e2 = ecs::entity();
    REQUIRE(ecs::count<Text>() == 0);

    REQUIRE(not ecs::has<Text>(e1));
    ecs::emplace<Text>(e1, "some text 1");
    REQUIRE(ecs::has<Text>(e1));
    REQUIRE(ecs::count<Text>() == 1);

    REQUIRE(not ecs::has<Text>(e2));
    ecs::emplace<Text>(e2, "some text 2");
    REQUIRE(ecs::has<Text>(e2));
    REQUIRE(ecs::count<Text>() == 2);
}

TEST_CASE("ecs access (get, span, with, for_each)")
{
    auto const e1 = ecs::entity();
    auto const e2 = ecs::entity();

    ecs::emplace<Text>(e1, "some text 1");
    REQUIRE(ecs::has<Text>(e1));
    REQUIRE(ecs::get<Text>(e1).data == "some text 1");
    REQUIRE(not ecs::has<Color>(e1));
    REQUIRE(std::ranges::equal(ecs::with<Text>(), std::set{e1}));

    ecs::insert(e2, Color{ .r = 1.0f, .g = 0.0f, .b = 0.0f });
    REQUIRE(ecs::has<Color>(e2));
    REQUIRE(ecs::get<Color>(e2) == Color{ .r = 1.0f, .g = 0.0f, .b = 0.0f });
    REQUIRE(not ecs::has<Text>(e2));
    REQUIRE(std::ranges::equal(ecs::with<Text>(), std::set{e1}));

    ecs::emplace<Text>(e2, "some text 2");
    REQUIRE(ecs::has<Text>(e2));
    REQUIRE(ecs::get<Text>(e2).data == "some text 2");
    REQUIRE(std::ranges::equal(ecs::with<Text>(), std::set{e1, e2}));

    REQUIRE(std::ranges::is_permutation(ecs::span<Text>(), std::array{
        Text{"some text 1"}, Text{"some text 2"}
    }));

    std::set<std::string> text;
    ecs::for_each<Text>([&](auto eid, auto const& value){ text.insert(value.data); });
    REQUIRE(text == std::set<std::string>{"some text 1", "some text 2"});
}

TEST_CASE("ecs modifiers (emplace, insert, remove, take)")
{
    auto const e1 = ecs::entity();
    REQUIRE(not ecs::has<Text>(e1));
    REQUIRE(not ecs::has<Color>(e1));

    ecs::emplace<Text>(e1, "some text");
    REQUIRE(ecs::has<Text>(e1));
    REQUIRE(ecs::get<Text>(e1).data == "some text");
    REQUIRE(not ecs::has<Color>(e1));

    ecs::insert(e1, Color{ .r = 1.0f, .g = 0.0f, .b = 0.0f });
    REQUIRE(ecs::has<Text>(e1));
    REQUIRE(ecs::get<Text>(e1).data == "some text");
    REQUIRE(ecs::has<Color>(e1));
    REQUIRE(ecs::get<Color>(e1) == Color{ .r = 1.0f, .g = 0.0f, .b = 0.0f });

    auto const text = ecs::take<Text>(e1);
    REQUIRE(text.data == "some text");
    REQUIRE(not ecs::has<Text>(e1));
    REQUIRE(ecs::has<Color>(e1));
    REQUIRE(ecs::get<Color>(e1) == Color{ .r = 1.0f, .g = 0.0f, .b = 0.0f });

    ecs::remove<Color>(e1);
    REQUIRE(not ecs::has<Text>(e1));
    REQUIRE(not ecs::has<Color>(e1));
}

TEST_CASE("ecs modifiers (erase, clear)")
{
    auto const e1 = ecs::entity();
    auto const e2 = ecs::entity();

    ecs::emplace<Text>(e1, "some text 1");
    ecs::emplace<Text>(e2, "some text 2");

    ecs::emplace<Color>(e1, Color{ .r = 1.0f, .g = 0.0f, .b = 0.0f });
    ecs::emplace<Color>(e2, Color{ .r = 1.0f, .g = 0.0f, .b = 0.0f });

    REQUIRE(ecs::has<Text>(e1));
    REQUIRE(ecs::has<Text>(e2));
    REQUIRE(ecs::has<Color>(e1));
    REQUIRE(ecs::has<Color>(e2));

    SECTION("erase") {
        ecs::erase<Text>();
        REQUIRE(not ecs::has<Text>(e1));
        REQUIRE(not ecs::has<Text>(e2));
        REQUIRE(ecs::has<Color>(e1));
        REQUIRE(ecs::has<Color>(e2));
    }

    SECTION("clear") {
        ecs::clear();
        REQUIRE(not ecs::has<Text>(e1));
        REQUIRE(not ecs::has<Text>(e2));
        REQUIRE(not ecs::has<Color>(e1));
        REQUIRE(not ecs::has<Color>(e2));
    }
}
