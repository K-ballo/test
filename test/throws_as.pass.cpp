// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

namespace {
struct my_error
{};

struct my_derived_error : my_error
{};
} // namespace

TEST_CASE(check_throws_as_int, "CHECK_THROWS_AS passes for exact type int")
{
    CHECK_THROWS_AS(int, throw 42);
}

TEST_CASE(
    require_throws_as_int, "REQUIRE_THROWS_AS passes without stopping the test"
)
{
    REQUIRE_THROWS_AS(int, throw 42);
    CHECK(true);
}

TEST_CASE(
    check_throws_as_std_exception, "CHECK_THROWS_AS passes for exact std type"
)
{
    CHECK_THROWS_AS(std::runtime_error, throw std::runtime_error{"oops"});
}

TEST_CASE(
    require_throws_as_std_exception,
    "REQUIRE_THROWS_AS passes without stopping the test"
)
{
    REQUIRE_THROWS_AS(std::runtime_error, throw std::runtime_error{"oops"});
    CHECK(true);
}

TEST_CASE(
    check_throws_as_derived_std_exception,
    "CHECK_THROWS_AS passes for derived std type"
)
{
    CHECK_THROWS_AS(std::exception, throw std::runtime_error{"oops"});
}

TEST_CASE(
    require_throws_as_derived_std_exception,
    "REQUIRE_THROWS_AS passes without stopping the test"
)
{
    REQUIRE_THROWS_AS(std::exception, throw std::runtime_error{"oops"});
    CHECK(true);
}

TEST_CASE(
    check_throws_as_custom_exception,
    "CHECK_THROWS_AS passes for exact custom type"
)
{
    CHECK_THROWS_AS(my_error, throw my_error{});
}

TEST_CASE(
    require_throws_as_custom_exception,
    "REQUIRE_THROWS_AS passes without stopping the test"
)
{
    REQUIRE_THROWS_AS(my_error, throw my_error{});
    CHECK(true);
}
