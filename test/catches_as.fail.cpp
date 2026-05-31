// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

TEST_CASE(
    check_catches_as_no_throw,
    "CHECK_CATCHES_AS fails when expression does not throw"
)
{
    CHECK_CATCHES_AS(std::runtime_error, 0)
    {
        CHECK(false); // body must not run
    }
}

TEST_CASE(
    require_catches_as_no_throw, "REQUIRE_CATCHES_AS fails and stops execution "
                                 "when expression does not throw"
)
{
    REQUIRE_CATCHES_AS(std::runtime_error, static_cast<void>(0))
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
