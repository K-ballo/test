// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cassert> // IWYU pragma: keep
#include <cstdlib> // IWYU pragma: keep

// EGGS_TEST_ASSERT(condition, message)
#ifdef EGGS_TEST_COVERAGE
// exit cleanly so atexit handlers run
#    define EGGS_TEST_ASSERT(condition, message) \
        do {                                     \
            if (!(condition)) [[unlikely]]       \
                std::exit(EXIT_FAILURE);         \
        } while (false)
#else
#    define EGGS_TEST_ASSERT(condition, message) \
        do {                                     \
            assert((condition) && message);      \
            if (!(condition)) [[unlikely]]       \
                std::abort();                    \
        } while (false)
#endif
