// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

TEST_CASE(
    check_throws_no_throw, "CHECK_THROWS fails when expression does not throw"
)
{
    CHECK_THROWS(static_cast<void>(1 + 1));
}

TEST_CASE(require_throws_no_throw, "REQUIRE_THROWS fails and stops execution")
{
    REQUIRE_THROWS(static_cast<void>(0));
    CHECK(false); // must not be reached
}

TEST_CASE(
    check_throws_require_propagated,
    "REQUIRE failing inside CHECK_THROWS propagates out"
)
{
    CHECK_THROWS([&] {
        REQUIRE(1 + 1 == 3);
    }());
    CHECK(false); // must not be reached
}
