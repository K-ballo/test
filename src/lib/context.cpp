// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/context.hpp>

#include <cctype>

namespace eggs::test::detail {

namespace {

// Whole-token check: every character must belong to the numeric literal
// (digits, hex/suffix letters, '.', digit separators) except for a '+'/'-'
// immediately following an exponent marker ('e', 'E', 'p', 'P'). Anything
// else (a space, an operator, an identifier character) means this is an
// expression that merely starts with a digit or sign, not a literal.
bool is_numeric_literal(std::string_view text)
{
    std::size_t i = 0;
    if (text[i] == '+' || text[i] == '-') ++i;
    if (i >= text.size() || !std::isdigit(static_cast<unsigned char>(text[i])))
        return false;

    for (; i < text.size(); ++i) {
        auto const c = static_cast<unsigned char>(text[i]);
        if (std::isalnum(c) || c == '.' || c == '\'') continue;
        if (c == '+' || c == '-') {
            auto const prev = static_cast<unsigned char>(text[i - 1]);
            if (prev == 'e' || prev == 'E' || prev == 'p' || prev == 'P')
                continue;
        }
        return false;
    }
    return true;
}

} // namespace

bool context_frame::is_literal(std::string_view expr) noexcept
{
    if (expr.empty()) return false;

    char const c = expr.front();
    if ((c == '"' || c == '\'') && expr.size() > 1 && expr.back() == c)
        return true;
    if (expr == "true" || expr == "false") return true;
    if (std::isdigit(static_cast<unsigned char>(c)) || c == '+' || c == '-')
        return is_numeric_literal(expr);

    return false;
}

} // namespace eggs::test::detail
