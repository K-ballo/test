// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <string_view>
#include <vector>

TEST_CASE(arithmetic, "basic arithmetic checks")
{
    CHECK(1 + 1 == 2);
    CHECK(3 * 4 == 12);
    REQUIRE(10 / 2 == 5);
    CHECK(7 - 3 == 4);
}

TEST_CASE(string_view, "string comparison checks")
{
    using namespace std::string_view_literals;
    std::string_view greeting = "hello";
    CHECK(greeting == "hello"sv);
    CHECK(greeting.size() == 5);
    REQUIRE(!greeting.empty());
    CHECK(greeting.front() == 'h');
}

TEST_CASE(containers, "basic container checks")
{
    std::vector<int> const v = {1, 2, 3};
    REQUIRE(!v.empty());
    CHECK(v.front() == 1);
    CHECK(v.back() == 3);

    // CHECK returns bool, so it can guard a further check that would be
    // unsafe to evaluate if the condition it depends on didn't hold.
    if (CHECK(v.size() >= 3)) {
        CHECK(v[2] == 3);
    }
}
