// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/noinline.hpp>

#include <stdexcept>

// Exercises CONTEXT alongside a FAILED diagnostic.

static int does_not_throw()
{
    return 0;
}

TEST_CASE(context_check_fail, "CONTEXT appears in a FAILED diagnostic")
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK(id == 7);
}

TEST_CASE(context_require_fail, "CONTEXT appears alongside a failing REQUIRE")
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE(id == 7);
    CHECK(false); // must not be reached
}

TEST_CASE(
    context_check_throws_fail,
    "CONTEXT appears alongside a failing CHECK_THROWS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK_THROWS(does_not_throw());
}

TEST_CASE(
    context_require_throws_fail,
    "CONTEXT appears alongside a failing REQUIRE_THROWS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE_THROWS(does_not_throw());
    CHECK(false); // must not be reached
}

TEST_CASE(
    context_check_throws_as_fail,
    "CONTEXT appears alongside a failing CHECK_THROWS_AS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK_THROWS_AS(std::runtime_error, does_not_throw());
}

TEST_CASE(
    context_require_throws_as_fail,
    "CONTEXT appears alongside a failing REQUIRE_THROWS_AS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE_THROWS_AS(std::runtime_error, does_not_throw());
    CHECK(false); // must not be reached
}

TEST_CASE(
    context_check_catches_as_fail,
    "CONTEXT appears alongside a failing CHECK_CATCHES_AS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK_CATCHES_AS(std::runtime_error, does_not_throw())
    {
        CHECK(false); // body must not run
    }
}

TEST_CASE(
    context_require_catches_as_fail,
    "CONTEXT appears alongside a failing REQUIRE_CATCHES_AS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE_CATCHES_AS(std::runtime_error, does_not_throw())
    {
        CHECK(false); // body must not run
    }
    CHECK(false); // must not be reached
}

TEST_CASE(
    context_check_nothrow_fail,
    "CONTEXT appears alongside a failing CHECK_NOTHROW"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK_NOTHROW(throw id);
}

TEST_CASE(
    context_require_nothrow_fail,
    "CONTEXT appears alongside a failing REQUIRE_NOTHROW"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE_NOTHROW(throw id);
    CHECK(false); // must not be reached
}

TEST_CASE(
    context_loop_fail,
    "CONTEXT declared inside a loop reflects only the current iteration"
)
{
    for (int i = 0; i != 3; ++i) {
        CONTEXT("i={}", i);
        CHECK(i != 1);
    }
}
