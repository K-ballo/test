// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <initializer_list>
#include <string_view>

namespace eggs::test {

// -- Option table -------------------------------------------------------------
//
// flag  - the flag stem
// value - the flag value
// desc  - --help right-column text

struct opt_spec
{
    std::string_view flag;
    std::string_view value;
    std::initializer_list<std::string_view> desc;
};

// MSVC has a tracking bug in constexpr global array with std::initializer_list
#if !defined(_MSC_VER) || defined(__clang__) // FIXME(MSVC): tested at 19.51
constexpr
#endif
    inline opt_spec const k_opts[] = {
        {.flag = "list",
         .value = {},
         .desc =
             {
                 "list selected test case names and exit;",
                 "respects --run filters",
             }},
        {.flag = "run=",
         .value = "<test_case>",
         .desc =
             {
                 "run only the specified test cases (repeatable).",
                 "all test cases will run if omitted",
             }},
        {.flag = "verbose",
         .value = {},
         .desc = {"also report successful checks"}},
};

} // namespace eggs::test
