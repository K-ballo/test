// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <concepts>

// TEST_CASE with no params auto-registers under its bare name; REGISTER_P
// cannot be used on it (see register_p.compile_fail.cpp).
TEST_CASE(stateless, "zero-parameter test")
{
    CHECK(1 + 1 == 2);
}

// Single-parameter test: each REGISTER_P passes one value directly.
TEST_CASE(square, "n squared is non-negative", int const& n)
{
    CHECK(n * n >= 0);
}

// Registers square/zero, square/positive, square/negative.
REGISTER_P(square, "zero", 0);
REGISTER_P(square, "positive", 42);
REGISTER_P(square, "negative", -7);

// Two-parameter test: REGISTER_P passes two values directly.
TEST_CASE(add, "addition is commutative", int const& a, int const& b)
{
    CHECK(a + b == b + a);
}

// Registers add/one-two, add/neg-pos.
REGISTER_P(add, "one-two", 1, 2);
REGISTER_P(add, "neg-pos", -3, 5);

// Single auto parameter: type is deduced from the value passed to REGISTER_P.
TEST_CASE(
    auto_single, "auto parameter deduces from REGISTER_P argument", auto n
)
{
    CHECK(n == n);
}

// Registers auto_single/int, auto_single/double, auto_single/bool.
REGISTER_P(auto_single, "int", 42);
REGISTER_P(auto_single, "double", 3.14);
REGISTER_P(auto_single, "bool", true);

// Multiple auto parameters: each deduces independently.
TEST_CASE(
    auto_multi, "multiple auto parameters deduce independently", auto a, auto b
)
{
    CHECK(a + b == b + a);
}

// Registers auto_multi/int-int, auto_multi/double-double, auto_multi/int-double.
REGISTER_P(auto_multi, "int-int", 1, 2);
REGISTER_P(auto_multi, "double-double", 1.0, 2.0);
REGISTER_P(auto_multi, "int-double", 1, 2.0);

// Constrained auto: std::integral enforced at the call site.
TEST_CASE(
    integral_auto, "std::integral auto only instantiates for integral types",
    std::integral auto n
)
{
    CHECK((n | 0) == n);
}

// Registers integral_auto/int, integral_auto/long, integral_auto/unsigned.
REGISTER_P(integral_auto, "int", 42);
REGISTER_P(integral_auto, "long", 42L);
REGISTER_P(integral_auto, "unsigned", 42u);
