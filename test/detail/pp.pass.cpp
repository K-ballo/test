// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/pp.hpp>

#define EGGS_TEST_DETAIL_PP_TEST_FOO foo
#define EGGS_TEST_DETAIL_PP_TEST_BAR bar

// EGGS_TEST_PP_CAT expands its arguments before pasting them together: a
// plain `a##b` would instead paste the macro names themselves, producing the
// literal token
// EGGS_TEST_DETAIL_PP_TEST_FOOEGGS_TEST_DETAIL_PP_TEST_BAR. Pasting into a
// macro name and checking its expansion with #if, rather than declaring a
// variable and checking it at runtime, keeps this a test of the
// preprocessor's behavior rather than of the compiler's.
#define foobar 42
#if EGGS_TEST_PP_CAT(                                          \
    EGGS_TEST_DETAIL_PP_TEST_FOO, EGGS_TEST_DETAIL_PP_TEST_BAR \
) != 42
#    error "EGGS_TEST_PP_CAT did not expand its arguments before pasting"
#endif
#undef foobar

// Non-macro arguments still paste directly, same as a plain ## would.
#define baz1 7
#if EGGS_TEST_PP_CAT(baz, 1) != 7
#    error "EGGS_TEST_PP_CAT did not paste non-macro arguments"
#endif
#undef baz1

int main()
{
    return 0;
}
