// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

// EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_PUSH / _POP
//
// Suppresses Clang's -Wglobal-constructors around a declaration that
// intentionally requires a global constructor (e.g. TEST_CASE's static
// registration).
//
//   EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_PUSH
//   inline static bool const registered_ = (...);
//   EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_POP
#if defined(__clang__)
#    define EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_PUSH \
        _Pragma("clang diagnostic push")                  \
            _Pragma("clang diagnostic ignored \"-Wglobal-constructors\"")
#    define EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_POP \
        _Pragma("clang diagnostic pop")
#else
#    define EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_PUSH
#    define EGGS_TEST_WARNING_NO_GLOBAL_CONSTRUCTORS_POP
#endif
