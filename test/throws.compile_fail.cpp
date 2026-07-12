// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#if defined(CHECK_THROWS_NOEXCEPT_COMPILE_FAIL)
TEST_CASE(
    check_throws_noexcept_compile_fail,
    "CHECK_THROWS rejects noexcept expressions"
)
{
    CHECK_THROWS(1 + 1);
}
#elif defined(REQUIRE_THROWS_NOEXCEPT_COMPILE_FAIL)
TEST_CASE(
    require_throws_noexcept_compile_fail,
    "REQUIRE_THROWS rejects noexcept expressions"
)
{
    REQUIRE_THROWS(1 + 1);
}
#endif
