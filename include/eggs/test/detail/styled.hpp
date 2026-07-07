// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test.hpp>

#include <algorithm>
#include <format>
#include <optional>
#include <string_view>

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
    std::optional<detail::color> fg;
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

inline constexpr text_style bold_red = fg(color::red) | emphasis::bold;
inline constexpr text_style bold_green = fg(color::green) | emphasis::bold;
inline constexpr text_style bold_cyan = fg(color::cyan) | emphasis::bold;
inline constexpr text_style yellow = fg(color::yellow);
inline constexpr text_style gray = fg(color::gray);

struct styled
{
    std::string_view text;
    text_style style;
};

// Decides whether output should be colored; has no side effect other than,
// best-effort, enabling ANSI escape processing on the Windows console when
// color is requested.
[[nodiscard]] bool should_use_color(color_when when) noexcept;

// Resolves to ts when use_color is true, or the empty text_style otherwise.
constexpr text_style when(bool use_color, text_style ts) noexcept
{
    return use_color ? ts : text_style{};
}

} // namespace eggs::test::detail

template <>
struct std::formatter<eggs::test::detail::styled>
    : std::formatter<std::string_view>
{
    using base = std::formatter<std::string_view>;

    auto format(eggs::test::detail::styled const& arg, auto& ctx) const
    {
        if (!arg.style.fg) {
            return base::format(arg.text, ctx);
        }

        auto out = std::format_to(
            ctx.out(), "\033[{};{}m", static_cast<int>(arg.style.em),
            static_cast<int>(*arg.style.fg)
        );
        ctx.advance_to(out);
        out = base::format(arg.text, ctx);
        return std::ranges::copy(std::string_view("\033[0m"), out).out;
    }
};
