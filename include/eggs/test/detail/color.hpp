// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <algorithm>
#include <cstdio>
#include <format>
#include <string_view>

#ifdef _WIN32
#    include <windows.h>
#else
#    include <unistd.h>
#endif

namespace eggs::test::detail {

namespace ansi {
inline constexpr const char* reset = "\033[0m";
inline constexpr const char* bold_red = "\033[1;31m";
inline constexpr const char* bold_green = "\033[1;32m";
inline constexpr const char* bold_cyan = "\033[1;36m";
inline constexpr const char* yellow = "\033[0;33m";
inline constexpr const char* gray = "\033[0;90m";
} // namespace ansi

enum class color_when
{
    auto_,
    always,
    never
};

inline bool g_use_color = false;

inline void init_color(color_when when) noexcept
{
    if (when == color_when::never) {
        g_use_color = false;
        return;
    }

    if (when == color_when::always) {
        g_use_color = true;
#ifdef _WIN32
        // Best-effort: enable ANSI escape processing for the console.
        // Do not disable color on failure — the user asked for it explicitly.
        HANDLE const h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode))
            SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
        return;
    }

    // auto_: enable color only when stdout is a console that supports ANSI.
#ifdef _WIN32
    HANDLE const h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    g_use_color = h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode) &&
                  SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
    g_use_color = isatty(fileno(stdout)) != 0;
#endif
}

struct colored
{
    std::string_view text;
    const char* code;
};

} // namespace eggs::test::detail

template <>
struct std::formatter<eggs::test::detail::colored>
    : std::formatter<std::string_view>
{
    auto format(eggs::test::detail::colored const& arg, auto& ctx) const
    {
        if (!eggs::test::detail::g_use_color) {
            return std::formatter<std::string_view>::format(arg.text, ctx);
        }

        auto out = std::ranges::copy(std::string_view(arg.code), ctx.out()).out;
        ctx.advance_to(out);
        out = std::formatter<std::string_view>::format(arg.text, ctx);
        out = std::ranges::copy(
                  std::string_view(eggs::test::detail::ansi::reset), out
        )
                  .out;
        return out;
    }
};
