// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

TEST_CASE(
    check_throws_as_no_throw,
    "CHECK_THROWS_AS fails when expression does not throw"
)
{
    CHECK_THROWS_AS(std::runtime_error, static_cast<void>(0));
}

TEST_CASE(
    require_throws_as_no_throw,
    "REQUIRE_THROWS_AS fails and stops execution when expression does not throw"
)
{
    REQUIRE_THROWS_AS(std::runtime_error, static_cast<void>(0));
    CHECK(false); // must not be reached
}

TEST_CASE(
    check_throws_as_wrong_type,
    "CHECK_THROWS_AS fails when expression throws a different type"
)
{
    CHECK_THROWS_AS(std::runtime_error, throw std::logic_error{"wrong"});
}

TEST_CASE(
    require_throws_as_wrong_type, "REQUIRE_THROWS_AS fails and stops execution "
                                  "when expression throws wrong type"
)
{
    REQUIRE_THROWS_AS(std::runtime_error, throw std::logic_error{"wrong"});
    CHECK(false); // must not be reached
}

TEST_CASE(
    check_throws_as_wrong_unknown,
    "CHECK_THROWS_AS fails when expression throws an unknown wrong type"
)
{
    CHECK_THROWS_AS(std::runtime_error, throw 42);
}

TEST_CASE(
    require_throws_as_wrong_unknown,
    "REQUIRE_THROWS_AS fails and stops "
    "execution when throwing unknown wrong type"
)
{
    REQUIRE_THROWS_AS(std::runtime_error, throw 42);
    CHECK(false); // must not be reached
}

TEST_CASE(
    check_throws_as_require_propagated,
    "REQUIRE failing inside CHECK_THROWS_AS propagates out"
)
{
    CHECK_THROWS_AS(std::runtime_error, [&] {
        REQUIRE(1 + 1 == 3);
    }());
    CHECK(false); // must not be reached
}
