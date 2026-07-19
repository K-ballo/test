// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <format>
#include <stdexcept>
#include <string>
#include <type_traits>

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

namespace {
struct my_error
{
    int value = 0;
};
} // namespace

TEST_CASE(
    check_throws_as_custom_exception,
    "CHECK_THROWS_AS passes for exact custom type"
)
{
    CHECK_THROWS_AS(my_error, throw my_error{42});
}

TEST_CASE(
    require_throws_as_custom_exception,
    "REQUIRE_THROWS_AS passes without stopping the test"
)
{
    REQUIRE_THROWS_AS(my_error, throw my_error{42});
    CHECK(true);
}

namespace {
struct my_derived_error : my_error
{};
} // namespace

TEST_CASE(
    check_throws_as_derived_custom_exception,
    "CHECK_THROWS_AS passes for derived custom type"
)
{
    CHECK_THROWS_AS(my_error, throw my_derived_error{42});
}

TEST_CASE(
    require_throws_as_derived_custom_exception,
    "REQUIRE_THROWS_AS passes without stopping the test"
)
{
    REQUIRE_THROWS_AS(my_error, throw my_derived_error{42});
    CHECK(true);
}

namespace {
struct my_formattable_error : my_error
{};
} // namespace

template <>
struct std::formatter<my_formattable_error> : std::formatter<std::string>
{
    template <typename FormatContext>
    auto format(my_formattable_error const& e, FormatContext& ctx) const
    {
        return std::formatter<std::string>::format(
            std::format("value {}", e.value), ctx
        );
    }
};

TEST_CASE(
    check_throws_as_formattable_udt,
    "CHECK_THROWS_AS passes for exact type with a custom formatter"
)
{
    CHECK_THROWS_AS(my_formattable_error, throw my_formattable_error{42});
}

TEST_CASE(
    require_throws_as_formattable_udt,
    "REQUIRE_THROWS_AS passes without stopping the test"
)
{
    REQUIRE_THROWS_AS(my_formattable_error, throw my_formattable_error{42});
    CHECK(true);
}

namespace {
struct my_final_error final : my_derived_error
{};
} // namespace

TEST_CASE(
    check_throws_as_final_exception,
    "CHECK_THROWS_AS passes for a final exception type"
)
{
    CHECK_THROWS_AS(my_final_error, throw my_final_error{42});
}

TEST_CASE(
    require_throws_as_final_exception,
    "REQUIRE_THROWS_AS passes without stopping the test"
)
{
    REQUIRE_THROWS_AS(my_final_error, throw my_final_error{42});
    CHECK(true);
}

TEST_CASE(
    check_throws_as_expression,
    "CHECK_THROWS_AS's result can be captured and rethrown"
)
{
    if (auto exc = CHECK_THROWS_AS(int, throw 42)) {
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
    require_throws_as_expression,
    "REQUIRE_THROWS_AS's result can be captured and rethrown"
)
{
    if (auto exc = REQUIRE_THROWS_AS(int, throw 42)) {
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
