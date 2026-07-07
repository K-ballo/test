// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/context.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/require.hpp>

#include <exception>
#include <format>
#include <source_location>
#include <string_view>
#include <vector>

// TEST_CASE(name, "description")
//
// Defines a struct named `name` with a static run() member and an inline
// static data member whose initialiser registers the test case before main().
// Follow the macro with a function body:
//
//   TEST_CASE(my_test, "adds two integers") {
//       CHECK(1 + 1 == 2);
//   }
#define TEST_CASE(name_, desc_)                            \
    struct name_                                           \
    {                                                      \
      private:                                             \
        static void run();                                 \
        inline static bool const registered_ =             \
            (::eggs::test::detail::registry::add({         \
                 .name = #name_,                           \
                 .desc = desc_,                            \
                 .run = &name_::run,                       \
                 .loc = ::std::source_location::current(), \
             }),                                           \
             true);                                        \
    };                                                     \
    void name_::run()

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

#define EGGS_TEST_PP_CAT_(a, b) a##b
#define EGGS_TEST_PP_CAT(a, b) EGGS_TEST_PP_CAT_(a, b)

// CONTEXT(expr)
//
// Records a scoped diagnostic message, shown alongside every CHECK/REQUIRE
// diagnostic (whether PASSED or FAILED) produced while still in scope.
// Nested CONTEXT calls all remain active until their own scope ends, and are
// shown outermost first:
//
//   void push_item(int id) {
//       CONTEXT(id);
//       CHECK(id > 0);
//   }
//
// Prints "<expr> -> <value>", where <value> is expr formatted with
// std::format at the point CONTEXT is called (not when a diagnostic is
// printed), so it reflects the state at the start of the scope even if expr
// changes, or is corrupted, before a diagnostic actually fires. When expr is
// itself a literal (e.g. a plain string message), the "<expr> -> " prefix is
// omitted and only the value is shown.
//
// Uses #__VA_ARGS__ so that expressions containing commas (e.g. template
// arguments) stringify correctly.
#define CONTEXT(...)                                     \
    ::eggs::test::detail::context_frame const            \
    EGGS_TEST_PP_CAT(eggs_test_context_, __LINE__)(      \
        #__VA_ARGS__, ::std::format("{}", (__VA_ARGS__)) \
    )

namespace eggs::test {

// Options passed to run()
struct run_options
{
    // ordered test case names; empty = run all
    std::vector<std::string_view> run;
    // list matching test case names instead of running them
    bool list = false;
    // also report successful checks
    bool verbose = false;
};

// Public entry point - call this from main().
int run(run_options opts = {});

// Full default main: parse_cli + --help handling + run.
// Link Eggs::TestMain to get this wired up automatically, or
// call it from your own main() to add setup before tests run.
int main(int argc, char const* argv[]);

inline int main(int argc, char* argv[])
{
    return main(argc, const_cast<char const**>(argv));
}

} // namespace eggs::test
