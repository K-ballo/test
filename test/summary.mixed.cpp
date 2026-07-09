// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// A mix of passing and failing TEST_CASEs.

TEST_CASE(mixed_pass_one, "first case passes")
{
    CHECK(true);
}

TEST_CASE(mixed_fail, "second case fails")
{
    CHECK(1 + 2 == 4);
}

TEST_CASE(mixed_pass_two, "third case passes")
{
    CHECK(true);
}

// A mix of passing and failing CHECKs in one test case.

TEST_CASE(check_mixed, "a mix of passing and failing CHECKs in one test case")
{
    CHECK(true);
    CHECK(1 + 2 == 4);
    CHECK(true);
}
