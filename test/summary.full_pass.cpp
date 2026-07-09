// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// Multiple TEST_CASEs that all pass.

TEST_CASE(full_pass_one, "first case passes")
{
    CHECK(true);
}

TEST_CASE(full_pass_two, "second case passes")
{
    CHECK(true);
}

TEST_CASE(full_pass_three, "third case passes")
{
    CHECK(true);
}
