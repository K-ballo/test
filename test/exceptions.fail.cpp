// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

TEST_CASE(
    leaked_std_exception,
    "std::exception leaked from a test case is caught and reported"
)
{
    throw std::runtime_error{"deliberate failure"};
}

TEST_CASE(
    leaked_unknown_exception,
    "unknown exception leaked from a test case is caught and reported"
)
{
    throw 42;
}

namespace {

[[noreturn]] bool throw_42()
{
    throw 42;
}

} // namespace

TEST_CASE(
    leaked_exception_check,
    "exception leaked from a CHECK is caught and reported"
)
{
    CHECK(throw_42());
    CHECK(false); // must not be reached
}

TEST_CASE(
    leaked_exception_require,
    "exception leaked from a REQUIRE is caught and reported"
)
{
    REQUIRE(throw_42());
    CHECK(false); // must not be reached
}
