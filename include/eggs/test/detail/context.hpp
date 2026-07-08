// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/run_state.hpp>

#include <format>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

namespace eggs::test::detail {

// Data recorded by CONTEXT: the message (already including its "expr -> "
// label, if any) and the CONTEXT(...) call's own source location. `message`
// is captured once, at construction (i.e. where CONTEXT(...) is written), so
// it reflects the state at that point even if the tracked value later
// changes or is corrupted before a diagnostic is printed. An aggregate:
// built via make_context() below and owned by a context_guard.
struct context_frame
{
    std::string message;
    std::source_location loc;
};

// RAII scope for a context_frame: owns it, and threads it onto an intrusive,
// singly-linked list rooted at the current run_state (innermost first),
// pushing on construction and popping on destruction. Nested guards all
// remain active until their own scope ends.
class context_guard
{
  public:
    explicit context_guard(context_frame frame) noexcept
        : frame(std::move(frame)),
          prev(run_state::current().context_top)
    {
        run_state::current().context_top = this;
    }

    ~context_guard() { run_state::current().context_top = prev; }

    context_guard(context_guard const&) = delete;
    context_guard& operator=(context_guard const&) = delete;

    context_frame const frame;
    context_guard const* const prev;
};

// Recognizes string, character, numeric, and boolean literal tokens (the
// whole token, not just its leading character — "5 + x" must not be mistaken
// for a literal on account of starting with a digit), so CONTEXT("message")
// doesn't repeat itself as `"message" -> message`.
[[nodiscard]] bool is_literal_expr(std::string_view expr) noexcept;

// Builds the context_frame for CONTEXT(expr): the value is formatted with a
// default "{}" spec and, unless expr is itself a literal token, prefixed with
// its own stringified expression (expr) as a label.
template <typename T>
context_frame
make_context(std::string_view expr, std::source_location loc, T&& value)
{
    return {
        .message = is_literal_expr(expr)
                       ? std::format("{}", std::forward<T>(value))
                       : std::format("{} -> {}", expr, std::forward<T>(value)),
        .loc = loc,
    };
}

// Builds the context_frame for CONTEXT(fmt, args...): fmt is used directly as
// the format string, with no auto-label — there is no single "expr" to show,
// the format string already is the message. std::format_string's compile-time
// format-string check applies here, at the CONTEXT(...) call site, since fmt
// is this overload's own (not forwarded-through) parameter.
//
// Overload resolution prefers the single-argument overload above whenever
// only one value is given (even if that value looks format-string-like,
// e.g. CONTEXT("a plain message")): binding a forwarding reference is a
// better match than the user-defined conversion to std::format_string this
// overload requires.
template <typename... Args>
context_frame make_context(
    std::string_view /*expr*/, std::source_location loc,
    std::format_string<Args...> fmt, Args&&... args
)
{
    return {
        .message = std::format(fmt, std::forward<Args>(args)...),
        .loc = loc,
    };
}

} // namespace eggs::test::detail
