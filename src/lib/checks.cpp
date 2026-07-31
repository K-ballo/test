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

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <format>
#include <source_location>
#include <string_view>
#include <typeinfo>
#include <utility>

#ifdef __cpp_lib_stacktrace
#    include <stacktrace>
#endif

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
// A frame belongs to this library if it comes from include/eggs/.
bool from_library(std::string_view file)
{
    auto const next_token = [](std::string_view s) {
        if (auto const sep = s.find_first_of("/\\");
            sep != std::string_view::npos) {
            return std::pair{s.substr(0, sep), s.substr(sep + 1)};
        }
        return std::pair{s, std::string_view{}};
    };

    while (!file.empty()) {
        auto const [token, rest] = next_token(file);
        if (token == "include" && next_token(rest).first == "eggs") return true;
        file = rest;
    }
    return false;
}

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
    for (std::size_t i = 1; i < limit; ++i) {
        auto const& e = st[i];

        auto const source_file = e.source_file();
        if (from_library(source_file)) continue;

        if (!source_file.empty()) {
            detail::println(
                stdout, "    #{} {}  [{}:{}]", i, e.description(), source_file,
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
void print_diagnostic(
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
    print_diagnostic(label_passed, info, "{}", expr);
}

void check_failed(const char* expr, diagnostic_info const& info)
{
    print_diagnostic(label_failed, info, "{}", expr);
}

void check_throws_failed(const char* expr, diagnostic_info const& info)
{
    print_diagnostic(label_failed, info, "{} did not throw", expr);
}

void check_throws_as_failed(
    const char* expr, const char* exc_type, std::exception_ptr const& threw,
    diagnostic_info const& info
)
{
    try {
        std::rethrow_exception(threw);
    } catch (std::exception const& exc) {
        print_diagnostic(
            label_failed, info,
            "{} threw unexpected exception ({}: \"{}\", expected {})", expr,
            typeid(exc).name(), exc.what(), exc_type
        );
    } catch (...) {
        print_diagnostic(
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
        print_diagnostic(
            label_failed, info, "{} threw unexpectedly ({}: \"{}\")", expr,
            typeid(exc).name(), exc.what()
        );
    } catch (...) {
        print_diagnostic(label_failed, info, "{} threw unexpectedly", expr);
    }
}

} // namespace eggs::test::detail
