// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/noinline.hpp>
#include <eggs/test/detail/print.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/run_state.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <cstddef>
#include <cstdio>
#include <exception>
#include <source_location>
#include <typeinfo>

namespace eggs::test::detail {

inline void
print_stacktrace(detail::stacktrace const& st, std::size_t entry_depth)
{
#ifdef __cpp_lib_stacktrace
    if (st.size() > entry_depth + 1) {
        detail::println(stdout, "  Stacktrace (innermost first):");
        std::size_t const user_frames = st.size() - entry_depth - 1;
        for (std::size_t i = 0; i < user_frames; ++i)
            detail::println(stdout, "    #{} {}", i, st[i].description());
    }
#else
    (void)st;
    (void)entry_depth;
#endif
}

EGGS_TEST_NOINLINE inline void check_failed(
    const char* expr, std::source_location const& loc, std::size_t entry_depth
)
{
    detail::println(
        stdout, "  FAILED: {}  [{}:{}]", expr, loc.file_name(), loc.line()
    );
    print_stacktrace(detail::stacktrace::current(1), entry_depth);
}

inline void throws_failed(
    const char* expr, detail::stacktrace const& st,
    std::source_location const& loc, std::size_t entry_depth
)
{
    detail::println(
        stdout, "  FAILED: {} did not throw  [{}:{}]", expr, loc.file_name(),
        loc.line()
    );
    print_stacktrace(st, entry_depth);
}

template <typename Fn>
EGGS_TEST_NOINLINE inline bool check_throws(
    Fn fn, const char* expr, run_state& s, std::source_location const& loc
)
{
    try {
        fn();
    } catch (require_failed const&) {
        throw;
    } catch (...) {
        ++s.assertions_passed;
        return true;
    }

    ++s.assertions_failed;
    throws_failed(expr, detail::stacktrace::current(1), loc, s.entry_depth);
    return false;
}

inline void throws_as_failed(
    const char* expr, detail::stacktrace const& st, const char* exc_type,
    std::exception_ptr threw, std::source_location const& loc,
    std::size_t entry_depth
)
{
    try {
        std::rethrow_exception(threw);
    } catch (std::exception const& exc) {
        detail::println(
            stdout,
            "  FAILED: {} threw unexpected exception ({}: \"{}\", expected "
            "{})"
            "  [{}:{}]",
            expr, typeid(exc).name(), exc.what(), exc_type, loc.file_name(),
            loc.line()
        );
    } catch (...) {
        detail::println(
            stdout,
            "  FAILED: {} threw unexpected exception (expected {})"
            "  [{}:{}]",
            expr, exc_type, loc.file_name(), loc.line()
        );
    }
    print_stacktrace(st, entry_depth);
}

template <typename ExcType, typename Fn>
EGGS_TEST_NOINLINE inline std::exception_ptr check_throws_as(
    Fn fn, const char* expr, run_state& s, const char* exc_type,
    std::source_location const& loc
)
{
    std::exception_ptr threw = nullptr;
    try {
        fn();
    } catch (require_failed const&) {
        throw;
    } catch (ExcType const&) {
        ++s.assertions_passed;
        return std::current_exception();
    } catch (...) {
        threw = std::current_exception();
    }

    ++s.assertions_failed;
    auto const st = stacktrace::current(1);
    if (threw)
        throws_as_failed(expr, st, exc_type, threw, loc, s.entry_depth);
    else
        throws_failed(expr, st, loc, s.entry_depth);
    return nullptr;
}

EGGS_TEST_NOINLINE inline void nothrow_failed(
    const char* expr, detail::stacktrace const& st, std::exception_ptr threw,
    std::source_location const& loc, std::size_t entry_depth
)
{
    try {
        std::rethrow_exception(threw);
    } catch (std::exception const& exc) {
        detail::println(
            stdout,
            "  FAILED: {} threw unexpectedly ({}: \"{}\")"
            "  [{}:{}]",
            expr, typeid(exc).name(), exc.what(), loc.file_name(), loc.line()
        );
    } catch (...) {
        detail::println(
            stdout,
            "  FAILED: {} threw unexpectedly"
            "  [{}:{}]",
            expr, loc.file_name(), loc.line()
        );
    }
    print_stacktrace(st, entry_depth);
}

template <typename Fn>
EGGS_TEST_NOINLINE inline bool check_nothrow(
    Fn fn, const char* expr, run_state& s, std::source_location const& loc
)
{
    std::exception_ptr threw = nullptr;
    try {
        fn();
        ++s.assertions_passed;
        return true;
    } catch (require_failed const&) {
        throw;
    } catch (...) {
        threw = std::current_exception();
    }

    ++s.assertions_failed;
    nothrow_failed(expr, stacktrace::current(1), threw, loc, s.entry_depth);
    return false;
}

} // namespace eggs::test::detail
