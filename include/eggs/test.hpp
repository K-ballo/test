// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/checks.hpp>    // IWYU pragma: export
#include <eggs/test/detail/context.hpp>   // IWYU pragma: export
#include <eggs/test/detail/pp.hpp>        // IWYU pragma: export
#include <eggs/test/detail/registry.hpp>  // IWYU pragma: export
#include <eggs/test/detail/run_state.hpp> // IWYU pragma: export
#include <eggs/test/detail/unwind.hpp>    // IWYU pragma: export
#include <eggs/test/detail/warning.hpp>   // IWYU pragma: export

#include <exception>       // IWYU pragma: keep
#include <format>          // IWYU pragma: keep
#include <source_location> // IWYU pragma: keep
#include <type_traits>     // IWYU pragma: keep

namespace eggs::test::detail {

template <typename T>
inline T require(T const& value)
{
    return static_cast<bool>(value) ? value : throw detail::unwind{};
}

} // namespace eggs::test::detail

// TEST_CASE(name, "description" [, params...])
//
// Defines a struct named `name` with a static run() member.
//
// Without required parameters the test auto-registers as a single case:
//   TEST_CASE(my_test, "adds two integers") { CHECK(1 + 1 == 2); }
//
// With required parameters no auto-registration happens; use REGISTER_P:
//   TEST_CASE(add, "adds two integers", int const& a, int const& b) {
//       CHECK(a + b == b + a);
//   }
//   REGISTER_P(add, "one-two", 1, 2);
#define TEST_CASE(name_, desc_, ...)                                    \
    static_assert(                                                      \
        ::eggs::test::detail::is_valid_instance_name(#name_),           \
        "TEST_CASE(" #name_ ", ...): name must be a valid identifier"   \
    );                                                                  \
    struct name_;                                                       \
    static_assert(                                                      \
        ::std::is_same_v<name_, ::name_>,                               \
        "TEST_CASE(" #name_ ", ...) must be defined at global scope"    \
    );                                                                  \
    struct name_                                                        \
    {                                                                   \
        static constexpr const char* case_desc_ = desc_;                \
        static void run(__VA_ARGS__);                                   \
        EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_PUSH                   \
        inline static auto const* reg_ = []<typename T = name_>()       \
            -> ::eggs::test::detail::test_entry const* {                \
            if constexpr (!::eggs::test::detail::is_parameterized<T>) { \
                return ::eggs::test::detail::registry::add({            \
                    #name_,                                             \
                    "" desc_,                                           \
                    [](::eggs::test::detail::run_state& state) {        \
                        state.mark_entry();                             \
                        return T::run();                                \
                    },                                                  \
                    ::std::source_location::current(),                  \
                });                                                     \
            } else {                                                    \
                return nullptr;                                         \
            }                                                           \
        }();                                                            \
        EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_POP                    \
    };                                                                  \
    void name_::run(__VA_ARGS__)

// REGISTER_P(name, "instance", args...)
//
// Registers one instance of the parameterized test case `name`, passing args
// directly to run().  Appears in the registry as "name/instance".
//
// `name` must be a parameterized TEST_CASE.
//
//   REGISTER_P(add, "small", 1, 2);
//   REGISTER_P(add, "negative", -3, 5);
#define REGISTER_P(name_, instance_, ...)                              \
    static_assert(                                                     \
        ::eggs::test::detail::is_valid_instance_name(#name_),          \
        "REGISTER_P(" #name_ ", ...): name must be a valid identifier" \
    );                                                                 \
    static_assert(                                                     \
        ::eggs::test::detail::is_parameterized<name_>,                 \
        "REGISTER_P can only be used with a parameterized TEST_CASE"   \
    );                                                                 \
    static_assert(                                                     \
        ::eggs::test::detail::is_valid_instance_name(instance_),       \
        "REGISTER_P(" #name_ ", " #instance_ ", " #__VA_ARGS__         \
        "): instance name must be a non-empty string of letters, "     \
        "digits, '_', '.', '/', '-', starting with a letter"           \
    );                                                                 \
    EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_PUSH                      \
    static auto const* EGGS_TEST_PP_CAT(eggs_test_reg_, __LINE__) = [] \
        -> ::eggs::test::detail::test_entry const* {                   \
            return ::eggs::test::detail::registry::add({               \
                #name_ "/" instance_,                                  \
                name_::case_desc_,                                     \
                [](::eggs::test::detail::run_state& state) {           \
                    state.mark_entry();                                \
                    return name_::run(__VA_ARGS__);                    \
                },                                                     \
                ::std::source_location::current(),                     \
            });                                                        \
        }() EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_POP

// CONTEXT(fmt, args...)
//
// Records a scoped diagnostic message, shown alongside every CHECK/REQUIRE
// diagnostic produced while still in scope. Nested CONTEXT calls all remain
// active until their scope ends.
//
// fmt is a std::format format string and args its arguments (possibly none):
//
//   void push_item(int id) {
//       CONTEXT("processing item {}", id);
//       CHECK(id > 0);
//   }
#define CONTEXT(...)                                                  \
    ::eggs::test::detail::context_frame const                         \
    EGGS_TEST_PP_CAT(eggs_test_context_frame_, __LINE__)(             \
        ::std::format(__VA_ARGS__), ::std::source_location::current() \
    )

// CHECK(expr)
//
// Evaluates expr (contextually converted to bool).  On failure prints a
// diagnostic with the source location and the textual expression.  When the
// check is inside a helper function (not directly in the TEST_CASE body) the
// diagnostic also includes a stacktrace of the user frames above it.
//
// Returns bool: true if the check passed, false if it failed.
//
// Uses #__VA_ARGS__ so that expressions containing commas (e.g. template
// arguments) stringify correctly.
#define CHECK(...) \
    ::eggs::test::detail::check(static_cast<bool>(__VA_ARGS__), #__VA_ARGS__)

// REQUIRE(expr)
//
// Identical to CHECK but stops execution of the current test case on failure.
// Returns bool.
#define REQUIRE(...) ::eggs::test::detail::require(CHECK(__VA_ARGS__))

// CHECK_THROWS(expr)
//
// Evaluates expr and passes if it throws any exception.  Fails with a
// diagnostic if expr completes without throwing.  Returns std::exception_ptr.
#define CHECK_THROWS(...)                                                \
    ::eggs::test::detail::check_throws(                                  \
        [&]() {                                                          \
            static_assert(                                               \
                !noexcept((__VA_ARGS__)),                                \
                "CHECK_THROWS(" #__VA_ARGS__ "): expression is noexcept" \
            );                                                           \
            (void)(__VA_ARGS__);                                         \
        },                                                               \
        #__VA_ARGS__                                                     \
    )

// REQUIRE_THROWS(expr)
//
// Identical to CHECK_THROWS but stops execution of the current test case on
// failure.  Returns std::exception_ptr.
#define REQUIRE_THROWS(...) \
    ::eggs::test::detail::require(CHECK_THROWS(__VA_ARGS__))

// CHECK_THROWS_AS(ExcType, expr)
//
// Evaluates expr and passes if it throws an exception of type ExcType (or a
// type derived from it).  Fails if nothing is thrown or the thrown type does
// not match.  Returns std::exception_ptr.
//
// ExcType is a single argument; template types containing commas require a
// using-alias.
#define CHECK_THROWS_AS(ExcType_, ...)                         \
    ::eggs::test::detail::check_throws_as<ExcType_>(           \
        [&]() {                                                \
            static_assert(                                     \
                !noexcept((__VA_ARGS__)),                      \
                "CHECK_THROWS_AS(" #ExcType_ ", " #__VA_ARGS__ \
                "): expression is noexcept"                    \
            );                                                 \
            (void)(__VA_ARGS__);                               \
        },                                                     \
        #__VA_ARGS__, #ExcType_                                \
    )

// REQUIRE_THROWS_AS(ExcType, expr)
//
// Identical to CHECK_THROWS_AS but stops execution of the current test case
// on failure.  Returns std::exception_ptr.
#define REQUIRE_THROWS_AS(ExcType_, ...) \
    ::eggs::test::detail::require(CHECK_THROWS_AS(ExcType_, __VA_ARGS__))

// CHECK_CATCHES_AS(ExcType, expr)
//
// Evaluates expr and passes if it throws an exception of type ExcType (or a
// type derived from it).  Fails if nothing is thrown or the type does not
// match.  The macro is followed by a catch body in which the caught exception
// is accessible as `exc`:
//
//   CHECK_CATCHES_AS(std::runtime_error, f()) {
//       CHECK(std::string_view(exc.what()) == "expected message");
//   }
//
// ExcType is a single argument; template types containing commas require a
// using-alias.
#define CHECK_CATCHES_AS(ExcType_, ...)                      \
    try {                                                    \
        if (auto e = CHECK_THROWS_AS(ExcType_, __VA_ARGS__)) \
            ::std::rethrow_exception(e);                     \
    } catch ([[maybe_unused]] ExcType_ const& exc)

// REQUIRE_CATCHES_AS(ExcType, expr)
//
// Identical to CHECK_CATCHES_AS but stops execution of the current test case
// on failure.
#define REQUIRE_CATCHES_AS(ExcType_, ...)                                   \
    try {                                                                   \
        ::std::rethrow_exception(REQUIRE_THROWS_AS(ExcType_, __VA_ARGS__)); \
    } catch ([[maybe_unused]] ExcType_ const& exc)

// CHECK_NOTHROW(expr)
//
// Evaluates expr and passes if it does not throw.  Fails with a diagnostic if
// any exception escapes.  Returns bool.
#define CHECK_NOTHROW(...)                           \
    ::eggs::test::detail::check_nothrow(             \
        [&]() { (void)(__VA_ARGS__); }, #__VA_ARGS__ \
    )

// REQUIRE_NOTHROW(expr)
//
// Identical to CHECK_NOTHROW but stops execution of the current test case on
// failure.  Returns bool.
#define REQUIRE_NOTHROW(...) \
    ::eggs::test::detail::require(CHECK_NOTHROW(__VA_ARGS__))
