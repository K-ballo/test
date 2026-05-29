// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/require.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <exception>
#include <ranges>
#include <source_location>
#include <string_view>
#include <tuple>
#include <vector>

namespace eggs::test::detail {

template <typename T>
bool auto_register(char const* name, char const* desc, std::source_location loc)
{
    if constexpr (requires { T::run(); }) {
        registry::add({
            name,
            desc,
            [](run_state& state) {
                state.entry_depth = stacktrace::current().size();
                T::run();
            },
            loc,
        });
    }
    return true;
}

} // namespace eggs::test::detail

// Internal helpers
#define _EGGS_CAT_(a, b) a##b
#define _EGGS_CAT(a, b) _EGGS_CAT_(a, b)

// TEST_CASE(name, "description" [, params...])
//
// Defines a struct named `name` with a static run() member.
//
// Without required params the test auto-registers as a single case:
//   TEST_CASE(my_test, "adds two integers") { CHECK(1 + 1 == 2); }
//
// With required params no auto-registration happens; use REGISTER_P / REGISTER_R:
//   TEST_CASE(add, "adds two integers", int const& a, int const& b) { CHECK(a + b == b + a); }
//   REGISTER_P(add, "one-two", 1, 2);
#define TEST_CASE(name_, desc_, ...)                             \
    struct name_                                                 \
    {                                                            \
        static constexpr const char* case_desc_ = desc_;         \
        static void run(__VA_ARGS__);                            \
        inline static bool const registered_ =                   \
            ::eggs::test::detail::auto_register<name_>(          \
                #name_, desc_, ::std::source_location::current() \
            );                                                   \
    };                                                           \
    void name_::run(__VA_ARGS__)

// REGISTER_P(name, "instance", args...)
//
// Registers one instance of the parameterized test case `name`, passing args
// directly to run().  Appears in the registry as "name/instance".
//
//   REGISTER_P(add, "small",    1,  2);
//   REGISTER_P(add, "negative", -3, 5);
#define REGISTER_P(name_, instance_, ...)                                \
    static bool const _EGGS_CAT(_eggs_reg_, __LINE__) = [] {             \
        ::eggs::test::detail::registry::add({                            \
            #name_ "/" instance_,                                        \
            name_::case_desc_,                                           \
            [](::eggs::test::detail::run_state& state) {                 \
                state.entry_depth =                                      \
                    ::eggs::test::detail::stacktrace ::current().size(); \
                return name_::run(__VA_ARGS__);                          \
            },                                                           \
            ::std::source_location::current(),                           \
        });                                                              \
        return true;                                                     \
    }()

namespace eggs::test::detail {

template <std::ranges::range... Rs>
auto make_ranges(Rs&&... rs)
{
    return std::make_tuple(std::forward<Rs>(rs)...);
}

template <typename T>
auto make_ranges(std::initializer_list<T> il)
{
    return std::make_tuple(std::vector<T>(il));
}

template <typename T, typename... Rest>
void make_ranges(std::initializer_list<T>, Rest&&...)
#if __cpp_deleted_function >= 202403L
    = delete ("only a single braced-init-list is supported; use "
              "std::vector{...} for multiple ranges");
#else
    = delete;
#endif

} // namespace eggs::test::detail

// REGISTER_R(name, "instance", range1 [, range2, ...])
//
// Registers one instance per element of the Cartesian product of all ranges,
// naming them "name/instance/_0", "name/instance/_1", etc.  For a single range the
// element is passed directly; for multiple ranges each tuple is unpacked via
// std::apply.  Ranges may be braced-init-lists ({1, 4, 9}), std::array,
// std::vector, or any other range; different ranges may have different types.
//
//   REGISTER_R(square, "small",  {1, 4, 9});
//   REGISTER_R(add,    "grid",   {0, 1, 2}, {0, 1, 2});
#define REGISTER_R(name_, instance_, ...)                                     \
    static bool const _EGGS_CAT(_eggs_reg_, __LINE__) = [] {                  \
        std::size_t _i_ = 0;                                                  \
        auto _rngs_ = ::eggs::test::detail::make_ranges(__VA_ARGS__);         \
        for (auto const& _v_ : std::apply(                                    \
                 [](auto const&... _rs_) {                                    \
                     return std::views::cartesian_product(_rs_...);           \
                 },                                                           \
                 _rngs_                                                       \
             )) {                                                             \
            auto _vals_ = std::apply(                                         \
                [](auto const&... _xs_) { return std::make_tuple(_xs_...); }, \
                _v_                                                           \
            );                                                                \
            ::eggs::test::detail::registry::add({                             \
                #name_ "/" instance_ "/_" + std::to_string(_i_),              \
                name_::case_desc_,                                            \
                [_vals_](::eggs::test::detail::run_state& state) {            \
                    return std::apply(                                        \
                        [&state](auto const&... _vs_) {                       \
                            state.entry_depth =                               \
                                ::eggs::test::detail::stacktrace ::current()  \
                                    .size();                                  \
                            return name_::run(_vs_...);                       \
                        },                                                    \
                        _vals_                                                \
                    );                                                        \
                },                                                            \
                ::std::source_location::current(),                            \
            });                                                               \
            ++_i_;                                                            \
        }                                                                     \
        return true;                                                          \
    }()

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

namespace eggs::test {

// Options passed to run()
struct run_options
{
    // ordered test case names; empty = run all
    std::vector<std::string_view> run;
    // list matching test case names instead of running them
    bool list = false;
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
