// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>
#include <string_view>

namespace {
struct my_error
{
    int value = 0;
};
} // namespace

TEST_CASE(
    check_catches_as_int,
    "CHECK_CATCHES_AS passes and binds int exception as exc"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(int, throw 42)
    {
        caught = true;
        CHECK(exc == 42);
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_int,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(int, throw 42)
    {
        caught = true;
        CHECK(exc == 42);
    }
    CHECK(caught);
}

TEST_CASE(
    check_catches_as_std_exception,
    "CHECK_CATCHES_AS passes and binds std exception as exc"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(std::runtime_error, throw std::runtime_error{"oops"})
    {
        caught = true;
        CHECK(std::string_view(exc.what()) == "oops");
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_std_exception,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(std::runtime_error, throw std::runtime_error{"oops"})
    {
        caught = true;
        CHECK(std::string_view(exc.what()) == "oops");
    }
    CHECK(caught);
}

TEST_CASE(
    check_catches_as_derived_std_exception,
    "CHECK_CATCHES_AS passes for derived std type"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(std::exception, throw std::runtime_error{"oops"})
    {
        caught = true;
        CHECK(std::string_view(exc.what()) == "oops");
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_derived_std_exception,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(std::exception, throw std::runtime_error{"oops"})
    {
        caught = true;
        CHECK(std::string_view(exc.what()) == "oops");
    }
    CHECK(caught);
}

TEST_CASE(
    check_catches_as_custom_exception,
    "CHECK_CATCHES_AS passes for exact custom type"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(my_error, throw my_error{})
    {
        caught = true;
        CHECK(exc.value == 0);
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_custom_exception,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(my_error, throw my_error{})
    {
        caught = true;
        CHECK(exc.value == 0);
    }
    CHECK(caught);
}

TEST_CASE(
    check_catches_as_empty_body,
    "CHECK_CATCHES_AS with empty body behaves like CHECK_THROWS_AS"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(std::runtime_error, throw std::runtime_error{"x"})
    {
        caught = true;
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_empty_body,
    "REQUIRE_CATCHES_AS with empty body does not stop the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(std::runtime_error, throw std::runtime_error{"x"})
    {
        caught = true;
    }
    CHECK(caught);
}
