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
} // namespace

TEST_CASE(check_throws_int, "CHECK_THROWS passes when expression throws int")
{
    CHECK_THROWS(throw 42);
}

TEST_CASE(require_throws_int, "REQUIRE_THROWS passes without stopping the test")
{
    REQUIRE_THROWS(throw 42);
    CHECK(true);
}

TEST_CASE(
    check_throws_std_exception,
    "CHECK_THROWS passes when expression throws std::exception"
)
{
    CHECK_THROWS(throw std::runtime_error{"oops"});
}

TEST_CASE(
    require_throws_std_exception,
    "REQUIRE_THROWS passes without stopping the test"
)
{
    REQUIRE_THROWS(throw std::runtime_error{"oops"});
    CHECK(true);
}

TEST_CASE(
    check_throws_custom_exception,
    "CHECK_THROWS passes when expression throws custom exception"
)
{
    CHECK_THROWS(throw my_error{});
}

TEST_CASE(
    require_throws_custom_exception,
    "REQUIRE_THROWS passes without stopping the test"
)
{
    REQUIRE_THROWS(throw my_error{});
    CHECK(true);
}
