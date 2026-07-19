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

// Exercises --verbose: every kind of successful check below must print
// "PASSED: <expr>" when --verbose is given, and nothing extra otherwise.
//
// CHECK_THROWS[_AS] additionally print what was caught: its formatted
// representation when formattable, else std::exception::what(), else
// "UNKNOWN EXCEPTION" -- mirroring registry::run's own EXCEPTION/UNKNOWN
// EXCEPTION wording for exceptions that escape a test case.

TEST_CASE(verbose_check_pass, "CHECK")
{
    CHECK(1 + 1 == 2);
}

TEST_CASE(
    verbose_throws_pass_std_exception, "CHECK_THROWS with a std::exception"
)
{
    CHECK_THROWS(throw std::runtime_error("thrown"));
}

TEST_CASE(
    verbose_throws_as_pass_formattable,
    "CHECK_THROWS_AS with a formattable builtin type"
)
{
    CHECK_THROWS_AS(int, throw 7);
}

namespace {
struct my_formattable_error
{
    int code;
};
} // namespace

template <>
struct std::formatter<my_formattable_error> : std::formatter<std::string>
{
    template <typename FormatContext>
    auto format(my_formattable_error const& e, FormatContext& ctx) const
    {
        return std::formatter<std::string>::format(
            std::format("code {}", e.code), ctx
        );
    }
};

TEST_CASE(
    verbose_throws_as_pass_formattable_udt,
    "CHECK_THROWS_AS with a formattable user-defined type"
)
{
    CHECK_THROWS_AS(my_formattable_error, throw my_formattable_error{5});
}

TEST_CASE(
    verbose_throws_as_pass_std_exception,
    "CHECK_THROWS_AS with a std::exception"
)
{
    CHECK_THROWS_AS(std::runtime_error, throw std::runtime_error("oops"));
}

namespace {
struct my_error
{};
} // namespace

TEST_CASE(
    verbose_throws_as_pass_unknown,
    "CHECK_THROWS_AS with an unformattable, non-std::exception type"
)
{
    CHECK_THROWS_AS(my_error, throw my_error{});
}

TEST_CASE(
    verbose_throws_pass_unknown,
    "CHECK_THROWS with a non-std::exception type: no type information at all"
)
{
    CHECK_THROWS(throw my_error{});
}

TEST_CASE(
    verbose_require_throws_pass_unknown,
    "REQUIRE_THROWS with a non-std::exception type: no type information at all"
)
{
    REQUIRE_THROWS(throw my_error{});
}

TEST_CASE(verbose_catches_as_pass, "CHECK_CATCHES_AS")
{
    CHECK_CATCHES_AS(std::runtime_error, throw std::runtime_error("caught")) {}
}

TEST_CASE(verbose_nothrow_pass, "CHECK_NOTHROW")
{
    CHECK_NOTHROW(1 + 1);
}
