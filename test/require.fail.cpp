// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

TEST_CASE(
    require_false_stops, "REQUIRE with false expression stops the test case"
)
{
    REQUIRE(1 + 2 == 4);
    // The runner must not reach CHECK(false); if it does the exit code
    // would still be 1 (two failures instead of one), but the test output
    // would reveal that REQUIRE did not stop execution.
    CHECK(false);
}
