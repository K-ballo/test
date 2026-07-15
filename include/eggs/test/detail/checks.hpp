// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/noinline.hpp>
#include <eggs/test/detail/require.hpp>
#include <eggs/test/detail/run_state.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <cstddef>
#include <exception>
#include <source_location>
#include <type_traits>

namespace eggs::test::detail {

void check_failed(
    const char* expr, std::source_location const& loc,
    detail::stacktrace const& st, std::size_t entry_depth
);

EGGS_TEST_NOINLINE inline bool check(
    bool c, const char* expr,
    run_state& s = ::eggs::test::detail::run_state::current(),
    std::source_location const& loc = std::source_location::current()
)
{
    if (c) {
        ++s.assertions_passed;
        return true;
    }

    ++s.assertions_failed;
    auto const& st = detail::stacktrace::current(1);
    check_failed(expr, loc, st, s.entry_depth);
    return false;
}

void check_throws_failed(
    const char* expr, std::source_location const& loc,
    detail::stacktrace const& st, std::size_t entry_depth
);

template <typename Fn>
EGGS_TEST_NOINLINE inline std::exception_ptr check_throws(
    Fn fn, const char* expr,
    run_state& s = ::eggs::test::detail::run_state::current(),
    std::source_location const& loc = std::source_location::current()
)
{
    try {
        fn();
    } catch (detail::unwind const&) {
        throw;
    } catch (...) {
        ++s.assertions_passed;
        return std::current_exception();
    }

    ++s.assertions_failed;
    auto const& st = detail::stacktrace::current(1);
    check_throws_failed(expr, loc, st, s.entry_depth);
    return nullptr;
}

void check_throws_as_failed(
    const char* expr, const char* exc_type, std::exception_ptr const& threw,
    std::source_location const& loc, detail::stacktrace const& st,
    std::size_t entry_depth
);

template <typename ExcType, typename Fn>
EGGS_TEST_NOINLINE inline std::exception_ptr check_throws_as(
    Fn fn, const char* expr, const char* exc_type,
    run_state& s = ::eggs::test::detail::run_state::current(),
    std::source_location const& loc = std::source_location::current()
)
{
    static_assert(
        !std::is_same_v<ExcType, detail::unwind>,
        "cannot catch eggs::test::detail::unwind"
    );

    std::exception_ptr threw = nullptr;
    try {
        fn();
    } catch (detail::unwind const&) {
        throw;
    } catch (ExcType const&) {
        ++s.assertions_passed;
        return std::current_exception();
    } catch (...) {
        threw = std::current_exception();
    }

    ++s.assertions_failed;
    auto const& st = detail::stacktrace::current(1);
    if (threw)
        check_throws_as_failed(expr, exc_type, threw, loc, st, s.entry_depth);
    else
        check_throws_failed(expr, loc, st, s.entry_depth);
    return nullptr;
}

void check_nothrow_failed(
    const char* expr, std::exception_ptr const& threw,
    std::source_location const& loc, detail::stacktrace const& st,
    std::size_t entry_depth
);

template <typename Fn>
EGGS_TEST_NOINLINE inline bool check_nothrow(
    Fn fn, const char* expr,
    run_state& s = ::eggs::test::detail::run_state::current(),
    std::source_location const& loc = std::source_location::current()
)
{
    std::exception_ptr threw = nullptr;
    try {
        fn();

        ++s.assertions_passed;
        return true;
    } catch (detail::unwind const&) {
        throw;
    } catch (...) {
        threw = std::current_exception();
    }

    ++s.assertions_failed;
    auto const& st = detail::stacktrace::current(1);
    check_nothrow_failed(expr, threw, loc, st, s.entry_depth);
    return false;
}

} // namespace eggs::test::detail
