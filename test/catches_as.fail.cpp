// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

static int does_not_throw()
{
    return 0;
}

TEST_CASE(
    check_catches_as_no_throw,
    "CHECK_CATCHES_AS fails when expression does not throw"
)
{
    CHECK_CATCHES_AS(std::runtime_error, does_not_throw())
    {
        CHECK(false); // body must not run
    }
}

TEST_CASE(
    require_catches_as_no_throw, "REQUIRE_CATCHES_AS fails and stops execution "
                                 "when expression does not throw"
)
{
    REQUIRE_CATCHES_AS(std::runtime_error, does_not_throw())
    {
        CHECK(false); // body must not run
    }
    CHECK(false); // must not be reached
}

TEST_CASE(
    check_catches_as_wrong_type,
    "CHECK_CATCHES_AS fails when expression throws a different type"
)
{
    CHECK_CATCHES_AS(std::runtime_error, throw std::logic_error{"wrong"})
    {
        CHECK(false); // body must not run
    }
}

TEST_CASE(
    require_catches_as_wrong_type, "REQUIRE_CATCHES_AS fails and stops "
                                   "execution when expression throws wrong type"
)
{
    REQUIRE_CATCHES_AS(std::runtime_error, throw std::logic_error{"wrong"})
    {
        CHECK(false); // body must not run
    }
    CHECK(false); // must not be reached
}

TEST_CASE(
    check_catches_as_wrong_unknown,
    "CHECK_CATCHES_AS fails when expression throws an unknown wrong type"
)
{
    CHECK_CATCHES_AS(std::runtime_error, throw 42)
    {
        CHECK(false); // body must not run
    }
}

TEST_CASE(
    require_catches_as_wrong_unknown,
    "REQUIRE_CATCHES_AS fails and stops "
    "execution when throwing unknown wrong type"
)
{
    REQUIRE_CATCHES_AS(std::runtime_error, throw 42)
    {
        CHECK(false); // body must not run
    }
    CHECK(false); // must not be reached
}

TEST_CASE(
    check_catches_as_require_propagated,
    "REQUIRE failing inside CHECK_CATCHES_AS propagates out"
)
{
    CHECK_CATCHES_AS(std::runtime_error, REQUIRE(1 + 2 == 4))
    {
        CHECK(false); // must not be reached
    }
    CHECK(false); // must not be reached
}

TEST_CASE(
    require_catches_as_require_propagated,
    "REQUIRE failing inside REQUIRE_CATCHES_AS propagates out"
)
{
    REQUIRE_CATCHES_AS(std::runtime_error, REQUIRE(1 + 2 == 4))
    {
        CHECK(false); // must not be reached
    }
    CHECK(false); // must not be reached
}
