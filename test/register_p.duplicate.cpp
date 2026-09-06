// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

TEST_CASE(duplicate_case, "desc", int const& a)
{
    CHECK(a == a);
}

// REGISTER_P rejects a duplicate instance name for the same test case.
REGISTER_P(duplicate_case, "same", 1);
REGISTER_P(duplicate_case, "same", 2);

// Selectable independently of the duplicate case.
TEST_CASE(unrelated_case, "desc")
{
    CHECK(true);
}
