// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/print.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <format>
#include <string>
#include <string_view>
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
    detail::print(stdout, "  FAILED: ");
    detail::println(stdout, fmt, std::forward<Args>(args)...);
    detail::println(
        stdout, "    #0 {}  [{}:{}]", loc.function_name(), loc.file_name(),
        loc.line()
    );
}

#ifdef __cpp_lib_stacktrace
std::filesystem::path library_root()
{
    static_assert(
        std::string_view(__FILE__).ends_with("src/lib/checks.cpp") ||
            std::string_view(__FILE__).ends_with("src\\lib\\checks.cpp"),
        "checks.cpp must live at src/lib/checks.cpp"
    );

    // frame[0] is always this function's own frame (checks.cpp).
    auto const& self = std::stacktrace::current();
    if (self.empty() || self[0].source_file().empty()) return {};

    return std::filesystem::path(self[0].source_file())
        .lexically_normal()
        .parent_path()  // lib/
        .parent_path()  // src/
        .parent_path(); // <library_root>/
}

bool from_library(
    std::stacktrace_entry const& e, std::filesystem::path const& lib
)
{
    auto const normalized =
        std::filesystem::path(e.source_file()).lexically_normal();

    auto const [lib_end, file_end] = std::mismatch(
        lib.begin(), lib.end(), normalized.begin(), normalized.end()
    );
    return lib_end == lib.end() && file_end != normalized.end() &&
           (*file_end == "src" || *file_end == "include");
}

// Prints "Stacktrace:" followed by one or more "<description>  [<file>:<line>]".
void print_stacktrace(detail::stacktrace const& st, std::size_t entry_depth)
{
    assert(st.size() >= entry_depth);
    if (st.size() <= entry_depth) return;

    std::size_t const limit = st.size() - entry_depth;
    if (limit <= 1) return;

    static auto const lib = library_root();

    // st[0] is always the CHECK/REQUIRE call site itself. Its location is
    // already printed above via source_location, so numbering and printing
    // start from the next frame.
    for (std::size_t i = 1; i < limit; ++i) {
        if (from_library(st[i], lib)) continue;

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
