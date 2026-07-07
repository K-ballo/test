// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/styled.hpp>

#include <cstdio>
#include <utility>

#ifdef _WIN32
#    include <windows.h>
#else
#    include <unistd.h>
#endif

namespace eggs::test::detail {

bool should_use_color(color_when when) noexcept
{
    switch (when) {
        case color_when::never: {
            return false;
        }

        case color_when::always: {
#ifdef _WIN32
            // Best-effort: enable ANSI escape processing for the console.
            HANDLE const h = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD mode = 0;
            if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode))
                SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
            return true;
        }

        case color_when::auto_: {
            // Enable color only when stdout is a console that supports ANSI.
#ifdef _WIN32
            HANDLE const h = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD mode = 0;
            return h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode) &&
                   SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
            return isatty(fileno(stdout)) != 0;
#endif
        }
    }

    std::unreachable();
}

} // namespace eggs::test::detail
