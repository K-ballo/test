// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

TEST_CASE(
    check_nothrow_throws_std_exception,
    "CHECK_NOTHROW fails when expression throws std exception"
)
{
    CHECK_NOTHROW(throw std::runtime_error{"oops"});
}

TEST_CASE(
    require_nothrow_throws_std_exception,
    "REQUIRE_NOTHROW fails and stops execution when expression throws"
)
{
    REQUIRE_NOTHROW(throw std::runtime_error{"oops"});
    CHECK(false); // must not be reached
}

TEST_CASE(
    check_nothrow_throws_unknown,
    "CHECK_NOTHROW fails when expression throws a unknown exception"
)
{
    CHECK_NOTHROW(throw 42);
}

TEST_CASE(
    require_nothrow_throws_unknown,
    "REQUIRE_NOTHROW fails and stops execution when expression throws non-std"
)
{
    REQUIRE_NOTHROW(throw 42);
    CHECK(false); // must not be reached
}

TEST_CASE(
    check_nothrow_require_propagated,
    "REQUIRE failing inside CHECK_NOTHROW propagates out"
)
{
    CHECK_NOTHROW([&] {
        REQUIRE(1 + 1 == 3);
    }());
    CHECK(false); // must not be reached
}

TEST_CASE(
    require_nothrow_require_propagated,
    "REQUIRE failing inside REQUIRE_NOTHROW propagates out"
)
{
    REQUIRE_NOTHROW([&] {
        REQUIRE(1 + 1 == 3);
    }());
    CHECK(false); // must not be reached
}
