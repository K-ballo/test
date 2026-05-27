// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/print.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/run_state.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <cstdio>
#include <source_location>

namespace eggs::test::detail {

inline void do_check_failed(
    run_state& s, const char* expr, std::source_location const& loc,
    detail::stacktrace const& st
)
{
    ++s.assertions_failed;
    detail::println(
        stdout, "  FAILED: {}  [{}:{}]", expr, loc.file_name(), loc.line()
    );

#ifdef __cpp_lib_stacktrace
    // Directly in test body:  st.size() == s.entry_depth + 1
    // Inside a helper:        st.size() >  s.entry_depth + 1
    if (st.size() > s.entry_depth + 1) {
        detail::println(stdout, "  Stacktrace (innermost first):");
        std::size_t const user_frames = st.size() - s.entry_depth - 1;
        for (std::size_t i = 0; i < user_frames; ++i)
            detail::println(stdout, "    #{} {}", i, st[i].description());
    }
#else
    (void)st;
#endif
}

inline void do_require_failed(
    run_state& s, const char* expr, std::source_location const& loc,
    detail::stacktrace const& st
)
{
    do_check_failed(s, expr, loc, std::move(st));
    throw require_failed{};
}

} // namespace eggs::test::detail
