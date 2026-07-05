// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <type_traits>

TEST_CASE(require_bool_true, "REQUIRE passes and does not stop execution")
{
    REQUIRE(true);
    REQUIRE(1 == 1);
    CHECK(true); // must still execute
}

TEST_CASE(require_pointer, "REQUIRE passes with non-null pointer")
{
    int x = 0;
    int* p = &x;
    REQUIRE(p != nullptr);
    CHECK(*p == x);
}

TEST_CASE(require_expression, "REQUIRE's result can be captured")
{
    if (auto r = REQUIRE(1 + 1 == 2)) {
        static_assert(std::is_same_v<decltype(r), bool>);

        CHECK(r == true);
    } else {
        CHECK(false); // must not be reached
    }
}
