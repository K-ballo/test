// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <concepts>
#include <vector>

// Zero-parameter instances: REGISTER_P with no value arguments.
TEST_CASE(stateless, "zero-parameter parametric test")
{
    CHECK(1 + 1 == 2);
}

REGISTER_P(stateless, "first");
REGISTER_P(stateless, "second");

// Single-parameter test: each REGISTER_P passes one value directly.
TEST_CASE(square, "n squared is non-negative", int const& n)
{
    CHECK(n * n >= 0);
}

REGISTER_P(square, "zero", 0);
REGISTER_P(square, "positive", 42);
REGISTER_P(square, "negative", -7);

// REGISTER_R with one range: registers square/small/_0 .. square/small/_3.
REGISTER_R(square, "small", std::vector<int>{1, 4, 9, 16});

// Braced-init-list: make_ranges deduces std::initializer_list<int> and
// materializes it into a vector, avoiding the cartesian_product deduction failure.
REGISTER_R(square, "brace", {1, 4, 9, 16});

// Two-parameter test: REGISTER_P passes two values; REGISTER_R cross-products two ranges.
TEST_CASE(add, "addition is commutative", int const& a, int const& b)
{
    CHECK(a + b == b + a);
}

REGISTER_P(add, "one-two", 1, 2);
REGISTER_P(add, "neg-pos", -3, 5);

// Registers 3x3 = 9 instances: add/grid/_0 .. add/grid/_8.
REGISTER_R(add, "grid", std::vector<int>{0, 1, 2}, std::vector<int>{0, 1, 2});

// Single auto parameter: type is deduced from the value passed to REGISTER_P.
TEST_CASE(
    auto_single, "auto parameter deduces from REGISTER_P argument", auto n
)
{
    CHECK(n == n);
}

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

REGISTER_P(integral_auto, "int", 42);
REGISTER_P(integral_auto, "long", 42L);
REGISTER_P(integral_auto, "unsigned", 42u);
