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
#include <string_view>

TEST_CASE(
    check_catches_as_int,
    "CHECK_CATCHES_AS passes and binds int exception as exc"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(int, throw 42)
    {
        caught = true;
        CHECK(exc == 42);
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_int,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(int, throw 42)
    {
        caught = true;
        CHECK(exc == 42);
    }
    CHECK(caught);
}

TEST_CASE(
    check_catches_as_std_exception,
    "CHECK_CATCHES_AS passes and binds std exception as exc"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(std::runtime_error, throw std::runtime_error{"oops"})
    {
        caught = true;
        CHECK(std::string_view(exc.what()) == "oops");
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_std_exception,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(std::runtime_error, throw std::runtime_error{"oops"})
    {
        caught = true;
        CHECK(std::string_view(exc.what()) == "oops");
    }
    CHECK(caught);
}

TEST_CASE(
    check_catches_as_derived_std_exception,
    "CHECK_CATCHES_AS passes for derived std type"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(std::exception, throw std::runtime_error{"oops"})
    {
        caught = true;
        CHECK(std::string_view(exc.what()) == "oops");
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_derived_std_exception,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(std::exception, throw std::runtime_error{"oops"})
    {
        caught = true;
        CHECK(std::string_view(exc.what()) == "oops");
    }
    CHECK(caught);
}

namespace {
struct my_error
{
    int value = 0;
};
} // namespace

TEST_CASE(
    check_catches_as_custom_exception,
    "CHECK_CATCHES_AS passes for exact custom type"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(my_error, throw my_error{42})
    {
        caught = true;
        CHECK(exc.value == 42);
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_custom_exception,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(my_error, throw my_error{42})
    {
        caught = true;
        CHECK(exc.value == 42);
    }
    CHECK(caught);
}

namespace {
struct my_derived_error : my_error
{};
} // namespace

TEST_CASE(
    check_catches_as_derived_custom_exception,
    "CHECK_CATCHES_AS passes and binds a derived custom exception as exc"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(my_error, throw my_derived_error{42})
    {
        caught = true;
        CHECK(exc.value == 42);
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_derived_custom_exception,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(my_error, throw my_derived_error{42})
    {
        caught = true;
        CHECK(exc.value == 42);
    }
    CHECK(caught);
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
    check_catches_as_formattable_udt,
    "CHECK_CATCHES_AS passes and binds an exception with a custom formatter"
)
{
    bool caught = false;
    CHECK_CATCHES_AS(my_formattable_error, throw my_formattable_error{42})
    {
        caught = true;
        CHECK(exc.value == 42);
    }
    CHECK(caught);
}

TEST_CASE(
    require_catches_as_formattable_udt,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    bool caught = false;
    REQUIRE_CATCHES_AS(my_formattable_error, throw my_formattable_error{42})
    {
        caught = true;
        CHECK(exc.value == 42);
    }
    CHECK(caught);
}

namespace {
struct my_final_error final : my_derived_error
{};
} // namespace

TEST_CASE(
    check_catches_as_final_exception,
    "CHECK_CATCHES_AS passes and binds a final exception type as exc"
)
{
    CHECK_CATCHES_AS(my_final_error, throw my_final_error{42})
    {
        CHECK(exc.value == 42);
    }
}

TEST_CASE(
    require_catches_as_final_exception,
    "REQUIRE_CATCHES_AS passes without stopping the test"
)
{
    REQUIRE_CATCHES_AS(my_final_error, throw my_final_error{42})
    {
        CHECK(exc.value == 42);
    }
    CHECK(true);
}

TEST_CASE(
    check_catches_as_empty_body,
    "CHECK_CATCHES_AS with empty body behaves like CHECK_THROWS_AS"
)
{
    CHECK_CATCHES_AS(std::runtime_error, throw std::runtime_error{"x"}) {}
}

TEST_CASE(
    require_catches_as_empty_body,
    "REQUIRE_CATCHES_AS with empty body does not stop the test"
)
{
    REQUIRE_CATCHES_AS(std::runtime_error, throw std::runtime_error{"x"}) {}
    CHECK(true);
}
