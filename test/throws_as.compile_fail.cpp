// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

#if defined(CHECK_THROWS_AS_NOEXCEPT_COMPILE_FAIL)
TEST_CASE(
    check_throws_as_noexcept_compile_fail,
    "CHECK_THROWS_AS rejects noexcept expressions"
)
{
    CHECK_THROWS_AS(std::runtime_error, 1 + 1);
}
#elif defined(REQUIRE_THROWS_AS_NOEXCEPT_COMPILE_FAIL)
TEST_CASE(
    require_throws_as_noexcept_compile_fail,
    "REQUIRE_THROWS_AS rejects noexcept expressions"
)
{
    REQUIRE_THROWS_AS(std::runtime_error, 1 + 1);
}
#endif
