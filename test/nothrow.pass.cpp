// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

TEST_CASE(
    check_nothrow_pass, "CHECK_NOTHROW passes when expression does not throw"
)
{
    CHECK_NOTHROW(1 + 1);
}

TEST_CASE(
    require_nothrow_pass, "REQUIRE_NOTHROW passes without stopping the test"
)
{
    REQUIRE_NOTHROW(1 + 1);
    CHECK(true);
}
