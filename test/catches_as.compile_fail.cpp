// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#ifdef CHECK_CATCHES_AS_NOEXCEPT_COMPILE_FAIL
#    include <stdexcept>

TEST_CASE(
    check_catches_as_noexcept_compile_fail,
    "CHECK_CATCHES_AS rejects noexcept expressions"
)
{
    CHECK_CATCHES_AS(std::runtime_error, 1 + 1) {}
}
#elifdef REQUIRE_CATCHES_AS_NOEXCEPT_COMPILE_FAIL
#    include <stdexcept>

TEST_CASE(
    require_catches_as_noexcept_compile_fail,
    "REQUIRE_CATCHES_AS rejects noexcept expressions"
)
{
    REQUIRE_CATCHES_AS(std::runtime_error, 1 + 1) {}
}
#elifdef CHECK_CATCHES_AS_UNWIND
#    include <eggs/test/detail/unwind.hpp>

TEST_CASE(
    check_catches_as_unwind, "CHECK_CATCHES_AS(unwind, ...) fails to compile"
)
{
    CHECK_CATCHES_AS(::eggs::test::detail::unwind, throw 2)
    {
        CHECK(false);
    }
}
#elifdef REQUIRE_CATCHES_AS_UNWIND
#    include <eggs/test/detail/unwind.hpp>

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
