// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <vector>

#if defined(REGISTER_R_NOT_A_RANGE_COMPILE_FAIL)

TEST_CASE(not_a_range, "n squared is non-negative", int const& n)
{
    CHECK(n * n >= 0);
}

// make_ranges requires every argument to satisfy std::ranges::range; a plain
// int is not iterable.
REGISTER_R(not_a_range, "bad", 42);

#elif defined(REGISTER_R_WRONG_ARITY_COMPILE_FAIL)

TEST_CASE(wrong_arity, "addition is commutative", int const& a, int const& b)
{
    CHECK(a + b == b + a);
}

// One range only supplies one value per instance; a two-parameter run()
// needs one range per parameter.
REGISTER_R(wrong_arity, "bad", std::vector<int>{1, 2, 3});

#elif defined(REGISTER_R_AUTO_REGISTERED_COMPILE_FAIL)

TEST_CASE(auto_registered, "zero-parameter test")
{
    CHECK(1 + 1 == 2);
}

// A TEST_CASE with no params already auto-registers under its bare name;
// REGISTER_R is only for TEST_CASEs with required params.
REGISTER_R(auto_registered, "second");

#endif
