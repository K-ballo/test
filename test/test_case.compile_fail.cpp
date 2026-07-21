// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#if defined(TEST_CASE_NO_BODY_COMPILE_FAIL)

// TEST_CASE must be followed by a function body.
TEST_CASE(no_body, "test case with no body")

#elif defined(TEST_CASE_NO_DESC_COMPILE_FAIL)

#    if defined(_MSC_VER) && !defined(__clang__)
#        if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
//           C4003: not enough arguments for function-like macro invocation
#            pragma warning(error : 4003)
#        endif
#    endif

// TEST_CASE requires a name and a description.
TEST_CASE(no_desc)
{
    CHECK(1 + 1 == 2);
}

#elif defined(TEST_CASE_INVALID_NAME_COMPILE_FAIL)

// TEST_CASE name must be an identifier.
TEST_CASE(invalid name, "test case with an invalid name")
{
    CHECK(1 + 1 == 2);
}

#elif defined(TEST_CASE_INVALID_DESC_COMPILE_FAIL)

// TEST_CASE description must be a string literal.
char const* const desc = "test case with an invalid description";

TEST_CASE(invalid_desc, desc)
{
    CHECK(1 + 1 == 2);
}

#endif
