// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

// EGGS_TEST_NOINLINE
//
// Prevents the compiler from inlining the annotated function at call sites.
// Apply this to helper functions called from test bodies to guarantee they
// always appear as distinct frames in a captured stacktrace, regardless of
// optimization level.
//
//   EGGS_TEST_NOINLINE void my_check_helper() {
//       CHECK(some_condition);
//   }
#ifdef _MSC_VER
#    define EGGS_TEST_NOINLINE __declspec(noinline)
#else
#    define EGGS_TEST_NOINLINE __attribute__((noinline))
#endif
