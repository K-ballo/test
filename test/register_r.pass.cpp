// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <vector>

// Single-parameter test: REGISTER_R with one range registers one instance per
// element.
TEST_CASE(square, "n squared is non-negative", int const& n)
{
    CHECK(n * n >= 0);
}

// Registers square/small/_0 .. square/small/_3.
REGISTER_R(square, "small", std::vector<int>{1, 4, 9, 16});

// Braced-init-list: make_ranges deduces std::initializer_list<int> and
// materializes it into a vector, avoiding the cartesian_product deduction
// failure.  Registers square/brace/_0 .. square/brace/_3.
REGISTER_R(square, "brace", {1, 4, 9, 16});

// Registers no instances: the range is empty, so the cartesian product has
// zero elements and the loop body never runs.
REGISTER_R(square, "none", std::vector<int>{});

// Two-parameter test: REGISTER_R cross-products two ranges.
TEST_CASE(add, "addition is commutative", int const& a, int const& b)
{
    CHECK(a + b == b + a);
}

// Registers 3x3 = 9 instances: add/grid/_0 .. add/grid/_8.
REGISTER_R(add, "grid", std::vector<int>{0, 1, 2}, std::vector<int>{0, 1, 2});
