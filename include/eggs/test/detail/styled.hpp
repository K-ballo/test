// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test.hpp>

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

enum class color : int
{
    red = 31,
    green = 32,
    yellow = 33,
    cyan = 36,
    gray = 90
};

enum class emphasis : int
{
    none = 0,
    bold = 1
};

struct text_style
{
    detail::color fg;
    detail::emphasis em = emphasis::none;
};

constexpr text_style fg(color c) noexcept
{
    return {c};
}

constexpr text_style operator|(text_style ts, emphasis em) noexcept
{
    ts.em = em;
    return ts;
}

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

struct styled
{
    std::string_view text;
    text_style style;
};

} // namespace eggs::test::detail

template <>
struct std::formatter<eggs::test::detail::styled>
    : std::formatter<std::string_view>
{
    auto format(eggs::test::detail::styled const& arg, auto& ctx) const
    {
        if (!eggs::test::detail::g_use_color) {
            return std::formatter<std::string_view>::format(arg.text, ctx);
        }

        auto out = std::format_to(
            ctx.out(), "\033[{};{}m", static_cast<int>(arg.style.em),
            static_cast<int>(arg.style.fg)
        );
        ctx.advance_to(out);
        out = std::formatter<std::string_view>::format(arg.text, ctx);
        return std::ranges::copy(std::string_view("\033[0m"), out).out;
    }
};
