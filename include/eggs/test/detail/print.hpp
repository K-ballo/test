// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <version>

#ifdef __cpp_lib_print
#    include <print>
#else
#    include <cstdio>
#    include <format>
#endif

namespace eggs::test::detail {

#ifdef __cpp_lib_print
using std::print;
using std::println;
#else
template <typename... Args>
void print(std::FILE* f, std::format_string<Args...> fmt, Args&&... args)
{
    std::fputs(std::format(fmt, std::forward<Args>(args)...).c_str(), f);
}

template <typename... Args>
void println(std::FILE* f, std::format_string<Args...> fmt, Args&&... args)
{
    print(f, fmt, std::forward<Args>(args)...);
    std::fputc('\n', f);
}
#endif

} // namespace eggs::test::detail
