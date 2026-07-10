// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>
#include <type_traits>

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

namespace {
struct my_error
{};
} // namespace

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

namespace {
struct my_final_error final : my_error
{};
} // namespace

TEST_CASE(
    check_throws_final_exception,
    "CHECK_THROWS passes when expression throws a final exception type"
)
{
    CHECK_THROWS(throw my_final_error{});
}

TEST_CASE(
    require_throws_final_exception,
    "REQUIRE_THROWS passes without stopping the test"
)
{
    REQUIRE_THROWS(throw my_final_error{});
    CHECK(true);
}

TEST_CASE(
    check_throws_expression,
    "CHECK_THROWS's result can be captured and rethrown"
)
{
    if (auto exc = CHECK_THROWS(throw 42)) {
        static_assert(std::is_same_v<decltype(exc), std::exception_ptr>);

        try {
            std::rethrow_exception(exc);
        } catch (int v) {
            CHECK(v == 42);
        }
    } else {
        CHECK(false); // must not be reached
    }
}

TEST_CASE(
    require_throws_expression,
    "REQUIRE_THROWS's result can be captured and rethrown"
)
{
    if (auto exc = REQUIRE_THROWS(throw 42)) {
        static_assert(std::is_same_v<decltype(exc), std::exception_ptr>);

        try {
            std::rethrow_exception(exc);
        } catch (int v) {
            CHECK(v == 42);
        }
    } else {
        CHECK(false); // must not be reached
    }
}
