// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdio>

// A minimal, always-on check for detail/ tests, which must not depend on
// the library they are testing. Unlike an assert, it does not abort on
// failure, so it must not guard against undefined behavior in the code
// that follows.
//
// main() should end with `return eggs::test_support::report();`.

namespace eggs::test_support {

inline int passed = 0;
inline int failed = 0;

inline int report()
{
    std::fprintf(stderr, "%d passed, %d failed\n", passed, failed);
    return failed != 0;
}

} // namespace eggs::test_support

#define EGGS_TEST_DETAIL_CHECK(...)                                      \
    do {                                                                 \
        if (static_cast<bool>(__VA_ARGS__)) {                            \
            ++eggs::test_support::passed;                                \
        } else {                                                         \
            std::fprintf(                                                \
                stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, \
                #__VA_ARGS__                                             \
            );                                                           \
            ++eggs::test_support::failed;                                \
        }                                                                \
    } while (false)
