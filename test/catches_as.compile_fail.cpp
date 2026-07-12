// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

#if defined(CHECK_CATCHES_AS_NOEXCEPT_COMPILE_FAIL)
TEST_CASE(
    check_catches_as_noexcept_compile_fail,
    "CHECK_CATCHES_AS rejects noexcept expressions"
)
{
    CHECK_CATCHES_AS(std::runtime_error, 1 + 1) {}
}
#elif defined(REQUIRE_CATCHES_AS_NOEXCEPT_COMPILE_FAIL)
TEST_CASE(
    require_catches_as_noexcept_compile_fail,
    "REQUIRE_CATCHES_AS rejects noexcept expressions"
)
{
    REQUIRE_CATCHES_AS(std::runtime_error, 1 + 1) {}
}
#elif defined(CHECK_CATCHES_AS_UNWIND)
TEST_CASE(
    check_catches_as_unwind, "CHECK_CATCHES_AS(unwind, ...) fails to compile"
)
{
    CHECK_CATCHES_AS(::eggs::test::detail::unwind, throw 2)
    {
        CHECK(false);
    }
}
#elif defined(REQUIRE_CATCHES_AS_UNWIND)
TEST_CASE(
    require_catches_as_unwind,
    "REQUIRE_CATCHES_AS(unwind, ...) fails to compile"
)
{
    REQUIRE_CATCHES_AS(::eggs::test::detail::unwind, throw 2)
    {
        CHECK(false);
    }
}
#endif
