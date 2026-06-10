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

EGGS_TEST_NOINLINE void check_failed(
    const char* expr, std::source_location const& loc, std::size_t entry_depth
)
{
    detail::println(
        stdout, "  FAILED: {}  [{}:{}]", expr, loc.file_name(), loc.line()
    );
    print_stacktrace(detail::stacktrace::current(1), entry_depth);
}

EGGS_TEST_NOINLINE void throws_failed(
    const char* expr, std::source_location const& loc, std::size_t entry_depth
)
{
    detail::println(
        stdout, "  FAILED: {} did not throw  [{}:{}]", expr, loc.file_name(),
        loc.line()
    );
    print_stacktrace(detail::stacktrace::current(2), entry_depth);
}

EGGS_TEST_NOINLINE void throws_as_failed(
    const char* expr, const char* exc_type, std::exception_ptr threw,
    std::source_location const& loc, std::size_t entry_depth
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
    print_stacktrace(detail::stacktrace::current(2), entry_depth);
}

EGGS_TEST_NOINLINE void nothrow_failed(
    const char* expr, std::exception_ptr threw, std::source_location const& loc,
    std::size_t entry_depth
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
    print_stacktrace(detail::stacktrace::current(2), entry_depth);
}

} // namespace eggs::test::detail
