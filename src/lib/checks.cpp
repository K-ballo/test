// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/print.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <cstdio>
#include <exception>
#include <typeinfo>

namespace eggs::test::detail {

namespace {

void print_stacktrace(detail::stacktrace const& st, std::size_t entry_depth)
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

} // namespace

EGGS_TEST_NOINLINE bool
check(bool c, const char* expr, run_state& s, std::source_location const& loc)
{
    if (c) {
        ++s.assertions_passed;
        return true;
    }

    ++s.assertions_failed;

    auto const& st = detail::stacktrace::current(1);
    detail::println(
        stdout, "  FAILED: {}  [{}:{}]", expr, loc.file_name(), loc.line()
    );
    print_stacktrace(st, s.entry_depth);
    return false;
}

EGGS_TEST_NOINLINE std::exception_ptr check_throws(
    detail::function_ref<void()> fn, const char* expr, run_state& s,
    std::source_location const& loc
)
{
    try {
        fn();
    } catch (require_failed const&) {
        throw;
    } catch (...) {
        ++s.assertions_passed;
        return std::current_exception();
    }

    ++s.assertions_failed;

    auto const& st = detail::stacktrace::current(1);
    detail::println(
        stdout, "  FAILED: {} did not throw  [{}:{}]", expr, loc.file_name(),
        loc.line()
    );
    print_stacktrace(st, s.entry_depth);
    return nullptr;
}

EGGS_TEST_NOINLINE void check_throws_as_failed(
    const char* expr, const char* exc_type, std::exception_ptr threw,
    std::source_location const& loc, detail::stacktrace const& st,
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

EGGS_TEST_NOINLINE std::exception_ptr check_throws_as(
    detail::function_ref<std::exception_ptr()> fn, const char* expr,
    const char* exc_type, run_state& s, std::source_location const& loc
)
{
    std::exception_ptr threw = nullptr;
    try {
        if (auto e = fn()) return e;
    } catch (require_failed const&) {
        throw;
    } catch (...) {
        threw = std::current_exception();
    }

    ++s.assertions_failed;

    auto const& st = detail::stacktrace::current(1);
    if (threw)
        check_throws_as_failed(expr, exc_type, threw, loc, st, s.entry_depth);
    else {
        detail::println(
            stdout, "  FAILED: {} did not throw  [{}:{}]", expr,
            loc.file_name(), loc.line()
        );
    }
    print_stacktrace(st, s.entry_depth);
    return nullptr;
}

inline void check_nothrow_failed(
    const char* expr, std::exception_ptr threw, std::source_location const& loc,
    detail::stacktrace const& st, std::size_t entry_depth
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

EGGS_TEST_NOINLINE bool check_nothrow(
    detail::function_ref<void()> fn, const char* expr, run_state& s,
    std::source_location const& loc
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

    auto const& st = detail::stacktrace::current(1);
    check_nothrow_failed(expr, threw, loc, st, s.entry_depth);
    return false;
}

} // namespace eggs::test::detail
