// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/run_state.hpp>

#include <string>
#include <string_view>
#include <utility>

namespace eggs::test::detail {

// A scoped diagnostic message pushed by CONTEXT; shown alongside every
// CHECK/REQUIRE diagnostic produced while it is alive. `message` is captured
// once, at construction (i.e. where CONTEXT(...) is written), so it reflects
// the state at that point even if the tracked value later changes or is
// corrupted before a diagnostic is printed.
class context_frame
{
  public:
    context_frame(std::string_view expr, std::string message)
        : _label(is_literal(expr) ? std::string_view{} : expr),
          _message(std::move(message))
    {
        run_state::current().context_stack.push_back(this);
    }

    ~context_frame() { run_state::current().context_stack.pop_back(); }

    context_frame(context_frame const&) = delete;
    context_frame& operator=(context_frame const&) = delete;

    [[nodiscard]] std::string_view label() const noexcept { return _label; }

    [[nodiscard]] std::string_view message() const noexcept { return _message; }

  private:
    // Recognizes string, character, numeric, and boolean literal tokens (the
    // whole token, not just its leading character — "5 + x" must not be
    // mistaken for a literal on account of starting with a digit), so
    // CONTEXT("message") doesn't repeat itself as `"message" -> message`.
    [[nodiscard]] static bool is_literal(std::string_view expr) noexcept;

    std::string_view _label;
    std::string _message;
};

} // namespace eggs::test::detail
