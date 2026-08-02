// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/context.hpp>
#include <eggs/test/detail/print.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <format>
#include <source_location>
#include <string_view>
#include <typeinfo>
#include <utility>

namespace eggs::test::detail {

namespace {

char const* const label_passed = "PASSED";
char const* const label_failed = "FAILED";

// Prints a single "CONTEXT:" header followed by one
// "<message>  [<file>:<line>]" line per active context call.
// Prints nothing if there are no active frames.
void print_context(context_frame const* frame)
{
    if (!frame) return;

    detail::println(stdout, "  CONTEXT:");
    for (; frame; frame = frame->prev) {
        detail::println(
            stdout, "    {}  [{}:{}]", frame->message, frame->loc.file_name(),
            frame->loc.line()
        );
    }
}

// Prints "<label>: <message>", "<function>  [<file>:<line>]".
template <typename... Args>
void print_outcome(
    const char* label, std::source_location const& loc,
    std::format_string<Args...> fmt, Args&&... args
)
{
    detail::print(stdout, "  {}: ", label);
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
    for (detail::stacktrace::size_type i = 1; i < limit; ++i) {
        auto const& e = st[i];
        if (from_library(e, lib)) continue;

        if (auto const& file = e.source_file(); !file.empty()) {
            detail::println(
                stdout, "    #{} {}  [{}:{}]", i, e.description(), file,
                e.source_line()
            );
        } else {
            detail::println(stdout, "    #{} {}", i, e.description());
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

template <typename... Args>
void print_check(
    const char* label, diagnostic_info const& info,
    std::format_string<Args...> fmt, Args&&... args
)
{
    print_context(info.ctx);
    print_outcome(label, info.loc, fmt, std::forward<Args>(args)...);
    print_stacktrace(info.st, info.entry_depth);
}

} // namespace

void check_passed(const char* expr, diagnostic_info const& info)
{
    print_check(label_passed, info, "{}", expr);
}

void check_failed(const char* expr, diagnostic_info const& info)
{
    print_check(label_failed, info, "{}", expr);
}

void check_throws_failed(const char* expr, diagnostic_info const& info)
{
    print_check(label_failed, info, "{} did not throw", expr);
}

void check_throws_as_failed(
    const char* expr, const char* exc_type, std::exception_ptr const& threw,
    diagnostic_info const& info
)
{
    try {
        std::rethrow_exception(threw);
    } catch (std::exception const& exc) {
        print_check(
            label_failed, info,
            "{} threw unexpected exception ({}: \"{}\", expected {})", expr,
            typeid(exc).name(), exc.what(), exc_type
        );
    } catch (...) {
        print_check(
            label_failed, info, "{} threw unexpected exception (expected {})",
            expr, exc_type
        );
    }
}

void check_nothrow_failed(
    const char* expr, std::exception_ptr const& threw,
    diagnostic_info const& info
)
{
    try {
        std::rethrow_exception(threw);
    } catch (std::exception const& exc) {
        print_check(
            label_failed, info, "{} threw unexpectedly ({}: \"{}\")", expr,
            typeid(exc).name(), exc.what()
        );
    } catch (...) {
        print_check(label_failed, info, "{} threw unexpectedly", expr);
    }
}

} // namespace eggs::test::detail
