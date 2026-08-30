// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdio>
#include <cstdlib>

// A basic, always-on assertion for detail/ tests, which must not depend on
// the library they are testing (in particular, not on
// eggs::test::detail::check, nor on TEST_CASE's registry-based machinery).
//
// Unlike <cassert>'s assert(), this is never disabled by NDEBUG, and it never
// risks MSVC's interactive debug-assert dialog (which would hang a headless
// ctest run): on failure it prints straight to stderr and calls
// std::abort(), bypassing the C runtime's own assert reporting entirely.
#define EGGS_TEST_DETAIL_ASSERT(...)                                         \
    do {                                                                     \
        if (!static_cast<bool>(__VA_ARGS__)) {                               \
            std::fprintf(                                                    \
                stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, \
                #__VA_ARGS__                                                 \
            );                                                               \
            std::abort();                                                    \
        }                                                                    \
    } while (false)
