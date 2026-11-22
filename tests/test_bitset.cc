#include <ecs/bitset.hh>

#include <catch2/catch_all.hpp>

TEST_CASE("bitset defaults and element access")
{
    ecs::bitset zero;
    REQUIRE_FALSE(zero.test(0));
    REQUIRE_FALSE(zero.test(127));
    REQUIRE(zero.none());
    REQUIRE_FALSE(zero.any());
    REQUIRE_FALSE(zero.all());

    ecs::bitset one;
    one.set_all(true);
    one.reserve(16);
    REQUIRE(one.test(0));
    REQUIRE(one.test(127));
    REQUIRE(one.any());
    REQUIRE(one.all());
    REQUIRE_FALSE(one.none());

    (void)(one[10] = false);
    REQUIRE_FALSE(one.test(10));
    REQUIRE(one.test(11));
    REQUIRE_FALSE(one.all());
}

TEST_CASE("bitset set, flip, and reference access")
{
    ecs::bitset bits;
    REQUIRE(bits.set(static_cast<size_t>(3)).test(3));
    REQUIRE(bits.test(2) == false);
    REQUIRE(bits.flip(3).test(3) == false);
    REQUIRE(bits.flip(3).test(3));

    bits.reserve(101);
    bits.set(100, false);
    REQUIRE_FALSE(bits.test(100));

    bits.set_all(false);
    REQUIRE(bits.none());
    REQUIRE_FALSE(bits.test(3));

    ecs::bitset const const_bits = ecs::bitset{}.set(static_cast<size_t>(5));
    REQUIRE(const_bits.test(5));
    REQUIRE(const_bits[5]);
}

TEST_CASE("bitset aggregate operations")
{
    ecs::bitset bits;
    bits.set(static_cast<size_t>(1));
    bits.set(static_cast<size_t>(3));
    REQUIRE_FALSE(bits.none());
    REQUIRE(bits.any());
    REQUIRE(bits.test(1));
    REQUIRE(bits.test(3));

    bits.flip_all();
    REQUIRE(bits.test(0));
    REQUIRE_FALSE(bits.test(1));
    REQUIRE(bits.test(2));
    REQUIRE_FALSE(bits.test(3));

    ecs::bitset flipped = ~bits;
    REQUIRE_FALSE(flipped.test(0));
    REQUIRE(flipped.test(1));
    REQUIRE_FALSE(flipped.test(2));
    REQUIRE(flipped.test(3));
}

TEST_CASE("bitset equality")
{
    ecs::bitset a;
    ecs::bitset b;
    REQUIRE(a == b);

    a.set(static_cast<size_t>(2));
    b.set(static_cast<size_t>(2));
    REQUIRE(a == b);

    b.set(static_cast<size_t>(9));
    REQUIRE_FALSE(a == b);

    ecs::bitset default_one;
    default_one.set_all(true);
    ecs::bitset cleared_one;
    cleared_one.set_all(true);
    cleared_one.reserve(5);
    cleared_one.set(4, false);
    REQUIRE(default_one != cleared_one);
}

TEST_CASE("bitset binary operations")
{
    ecs::bitset a;
    a.set(static_cast<size_t>(1)).set(static_cast<size_t>(2)).set(static_cast<size_t>(5));

    ecs::bitset b;
    b.set(static_cast<size_t>(2)).set(static_cast<size_t>(3)).set(static_cast<size_t>(5));

    REQUIRE((a & b).test(1) == false);
    REQUIRE((a & b).test(2));
    REQUIRE((a & b).test(3) == false);
    REQUIRE((a & b).test(5));

    REQUIRE((a | b).test(1));
    REQUIRE((a | b).test(2));
    REQUIRE((a | b).test(3));
    REQUIRE((a | b).test(5));

    ecs::bitset xor_result = a ^ b;
    REQUIRE(xor_result.test(1));
    REQUIRE_FALSE(xor_result.test(2));
    REQUIRE(xor_result.test(3));
    REQUIRE_FALSE(xor_result.test(5));

    ecs::bitset x;
    x.set_all(true);
    x.reserve(5);
    ecs::bitset y;
    y.set_all(true);
    y.reserve(5);
    y.set(3, false);
    ecs::bitset and_result = x & y;
    REQUIRE(and_result.test(0));
    REQUIRE_FALSE(and_result.test(3));
    REQUIRE(and_result.test(4));
}

TEST_CASE("bitset shifts")
{
    ecs::bitset bits;
    bits.set(static_cast<size_t>(1)).set(static_cast<size_t>(3));

    ecs::bitset left = bits << 2;
    REQUIRE_FALSE(left.test(1));
    REQUIRE(left.test(3));
    REQUIRE(left.test(5));
    REQUIRE_FALSE(left.test(7));

    ecs::bitset right = bits >> 1;
    REQUIRE(right.test(0));
    REQUIRE_FALSE(right.test(1));
    REQUIRE(right.test(2));
    REQUIRE_FALSE(right.test(3));

    ecs::bitset compound = bits;
    (void)(compound <<= 1);
    REQUIRE(compound.test(2));
    REQUIRE(compound.test(4));

    compound = bits;
    (void)(compound >>= 1);
    REQUIRE(compound.test(0));
    REQUIRE(compound.test(2));
}

TEST_CASE("bitset capacity and shrink to fit")
{
    ecs::bitset bits;
    REQUIRE(bits.capacity() >= 0);

    bits.reserve(70);
    REQUIRE(bits.capacity() >= 70);

    bits.set(static_cast<size_t>(3));
    bits.set(static_cast<size_t>(8));
    bits.set(static_cast<size_t>(69));
    bits.shrink_to_fit();
    REQUIRE(bits.test(3));
    REQUIRE(bits.test(8));
    REQUIRE(bits.test(69));
}

TEST_CASE("bitset storage uses low-indexed bytes first")
{
    ecs::bitset bits;
    bits.reserve(16);
    bits.set(static_cast<size_t>(11));

    REQUIRE(bits.test(11));
    REQUIRE_FALSE(bits.test(3));
    REQUIRE_FALSE(bits.test(12));
}

TEST_CASE("bitset sparse set, shift, and invert")
{
    ecs::bitset bits;
    REQUIRE(bits.test(0) == false);
    REQUIRE(bits.test(1) == false);
    REQUIRE(bits.test(1048576) == false);

    bits.set(0);
    REQUIRE(bits.test(0) == true);
    REQUIRE(bits.test(1) == false);
    REQUIRE(bits.test(1048576) == false);

    bits <<= 1;
    REQUIRE(bits.test(0) == false);
    REQUIRE(bits.test(1) == true);
    REQUIRE(bits.test(1048576) == false);

    bits = ~bits;
    REQUIRE(bits.test(0) == true);
    REQUIRE(bits.test(1) == false);
    REQUIRE(bits.test(1048576) == true);
}
