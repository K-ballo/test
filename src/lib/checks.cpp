// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/print.hpp>
#include <eggs/test/detail/stacktrace.hpp>
#include <eggs/test/detail/styled.hpp>

#include <cassert>
#include <cstdio>
#include <exception>
#include <format>
#include <string>
#include <typeinfo>

namespace eggs::test::detail {

namespace {

// Prints "FAILED: <message>" followed by "<function>  [<file>:<line>]".
template <typename... Args>
void print_failed(
    std::source_location const& loc, std::format_string<Args...> fmt,
    Args&&... args
)
{
    detail::print(
        stdout, "  {}: ",
        detail::styled(
            "FAILED", detail::fg(detail::color::red) | detail::emphasis::bold
        )
    );
    detail::println(stdout, fmt, std::forward<Args>(args)...);
    detail::println(
        stdout, "    {}  [{}:{}]", loc.function_name(), loc.file_name(),
        loc.line()
    );
}

#ifdef __cpp_lib_stacktrace
// Prints "Stacktrace:" followed by one or more "<description>  [<file>:<line>]".
void print_stacktrace(detail::stacktrace const& st, std::size_t entry_depth)
{
    assert(st.size() >= entry_depth);
    if (st.size() <= entry_depth) return;

    std::size_t const limit = st.size() - entry_depth;
    if (limit <= 1) return;

    // st[0] is always the CHECK/REQUIRE call site itself. Its location is
    // already printed above via source_location, so numbering and printing
    // start from the next frame.
    detail::println(stdout, "  Stacktrace (innermost first):");
    for (std::size_t i = 1; i < limit; ++i) {
        if (auto const& file = st[i].source_file(); !file.empty()) {
            detail::println(
                stdout, "    #{} {}  [{}:{}]", i, st[i].description(), file,
                st[i].source_line()
            );
        } else {
            detail::println(stdout, "    #{} {}", i, st[i].description());
        }
    }
}
#else
void print_stacktrace(
    detail::stacktrace const& /*st*/, std::size_t /*entry_depth*/
)
{
}
#endif

} // namespace

EGGS_TEST_NOINLINE void check_failed(
    const char* expr, std::source_location const& loc, std::size_t entry_depth
)
{
    print_failed(loc, "{}", expr);
    print_stacktrace(detail::stacktrace::current(1), entry_depth);
}

EGGS_TEST_NOINLINE void throws_failed(
    const char* expr, std::source_location const& loc, std::size_t entry_depth
)
{
    print_failed(loc, "{} did not throw", expr);
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
        print_failed(
            loc, "{} threw unexpected exception ({}: \"{}\", expected {})",
            expr, typeid(exc).name(), exc.what(), exc_type
        );
    } catch (...) {
        print_failed(
            loc, "{} threw unexpected exception (expected {})", expr, exc_type
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
        print_failed(
            loc, "{} threw unexpectedly ({}: \"{}\")", expr, typeid(exc).name(),
            exc.what()
        );
    } catch (...) {
        print_failed(loc, "{} threw unexpectedly", expr);
    }
    print_stacktrace(detail::stacktrace::current(2), entry_depth);
}

} // namespace eggs::test::detail
