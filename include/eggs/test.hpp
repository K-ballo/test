// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/run_state.hpp>
#include <eggs/test/detail/runner.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <source_location>

// TEST_CASE(name, "description")
//
// Defines a struct named `name` with a static run() member and an inline
// static data member whose initialiser registers the test case before main().
// Follow the macro with a function body:
//
//   TEST_CASE(my_test, "adds two integers") {
//       CHECK(1 + 1 == 2);
//   }
//
// All names are fully qualified (::eggs::test::detail::) so the macro is safe
// inside any user namespace.
#define TEST_CASE(name_, desc_)                                                \
    struct name_                                                               \
    {                                                                          \
        static void run();                                                     \
        inline static bool const registered_ = [] {                            \
            ::eggs::test::detail::registry::add({#name_, desc_, &name_::run}); \
            return true;                                                       \
        }();                                                                   \
    };                                                                         \
    void name_::run()

// CHECK(expr)
//
// Evaluates expr (contextually converted to bool).  On failure prints a
// diagnostic with the source location and the textual expression.  When the
// check is inside a helper function (not directly in the TEST_CASE body) the
// diagnostic also includes a stacktrace of the user frames above it.
//
// Uses #__VA_ARGS__ so that expressions containing commas (e.g. template
// arguments) stringify correctly.
#define CHECK(...)                                                          \
    do {                                                                    \
        if (static_cast<bool>(__VA_ARGS__))                                 \
            ++::eggs::test::detail::run_state::current().assertions_passed; \
        else                                                                \
            ::eggs::test::detail::do_check_failed(                          \
                ::eggs::test::detail::run_state::current(), #__VA_ARGS__,   \
                ::std::source_location::current(),                          \
                ::eggs::test::detail::stacktrace::current()                 \
            );                                                              \
    } while (false)

// REQUIRE(expr)
//
// Identical to CHECK but also throws eggs::test::detail::require_failed on
// failure, which is caught by the runner to stop execution of the current test
// case while allowing subsequent cases to continue.
#define REQUIRE(...)                                                        \
    do {                                                                    \
        if (static_cast<bool>(__VA_ARGS__))                                 \
            ++::eggs::test::detail::run_state::current().assertions_passed; \
        else                                                                \
            ::eggs::test::detail::do_require_failed(                        \
                ::eggs::test::detail::run_state::current(), #__VA_ARGS__,   \
                ::std::source_location::current(),                          \
                ::eggs::test::detail::stacktrace::current()                 \
            );                                                              \
    } while (false)

namespace eggs::test {

// Public entry point — call this from main().
inline int run()
{
    return ::eggs::test::detail::registry::run_all();
}

} // namespace eggs::test
