// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// Exercises CONTEXT alongside a PASSED diagnostic (with --verbose).

TEST_CASE(context_check_pass, "CONTEXT appears in a PASSED diagnostic")
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK(id == 42);
}

TEST_CASE(context_require_pass, "CONTEXT appears alongside a passing REQUIRE")
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE(id == 42);
}

TEST_CASE(
    context_check_throws_pass,
    "CONTEXT appears alongside a passing CHECK_THROWS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK_THROWS(throw id);
}

TEST_CASE(
    context_require_throws_pass,
    "CONTEXT appears alongside a passing REQUIRE_THROWS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE_THROWS(throw id);
}

TEST_CASE(
    context_check_throws_as_pass,
    "CONTEXT appears alongside a passing CHECK_THROWS_AS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK_THROWS_AS(int, throw id);
}

TEST_CASE(
    context_require_throws_as_pass,
    "CONTEXT appears alongside a passing REQUIRE_THROWS_AS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE_THROWS_AS(int, throw id);
}

TEST_CASE(
    context_check_catches_as_pass,
    "CONTEXT appears alongside a passing CHECK_CATCHES_AS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK_CATCHES_AS(int, throw id)
    {
        CHECK(exc == id);
    }
}

TEST_CASE(
    context_require_catches_as_pass,
    "CONTEXT appears alongside a passing REQUIRE_CATCHES_AS"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE_CATCHES_AS(int, throw id)
    {
        CHECK(exc == id);
    }
}

TEST_CASE(
    context_check_nothrow_pass,
    "CONTEXT appears alongside a passing CHECK_NOTHROW"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    CHECK_NOTHROW(id);
}

TEST_CASE(
    context_require_nothrow_pass,
    "CONTEXT appears alongside a passing REQUIRE_NOTHROW"
)
{
    int id = 42;
    CONTEXT("id={}", id);
    REQUIRE_NOTHROW(id);
}

TEST_CASE(context_fmt_pass, "CONTEXT(fmt, args...) prints the formatted result")
{
    int i = 1;
    int j = 2;
    CONTEXT("i={} j={}", i, j);
    CHECK(1 + 1 == 2);
}

TEST_CASE(
    context_sequential_pass, "multiple CONTEXT calls appear innermost first"
)
{
    CONTEXT("outer");
    CONTEXT("inner");
    CHECK(1 + 1 == 2);
}

TEST_CASE(context_scoped_pass, "CONTEXT only applies within its scope")
{
    {
        CONTEXT("scoped out");
    }
    CHECK(1 + 1 == 2);
}
