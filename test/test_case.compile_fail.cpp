// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#if defined(TEST_CASE_NO_BODY_COMPILE_FAIL)

// TEST_CASE expands to a function declarator; without a following { ... }
// body it is neither a valid declaration (missing ;) nor a definition.
TEST_CASE(no_body, "test case with no body")

#elif defined(TEST_CASE_NO_DESC_NO_PARAMS_COMPILE_FAIL)

// TEST_CASE requires at least a name and a description; omitting both the
// description and any params leaves too few macro arguments.
TEST_CASE(no_desc_no_params)
{
    CHECK(1 + 1 == 2);
}

#elif defined(TEST_CASE_NO_DESC_WITH_PARAMS_COMPILE_FAIL)

// Omitting the description but supplying params shifts the first param
// declaration into desc_'s position instead of failing outright: case_desc_
// ends up initialized from a declarator, run() loses its first parameter,
// and the body refers to a name that is no longer in scope.
TEST_CASE(no_desc_with_params, int const& a, int const& b)
{
    CHECK(a + b == b + a);
}

#elif defined(TEST_CASE_DEFAULTED_PARAM_COMPILE_FAIL)

// TEST_CASE expands to both a declaration (inside the struct) and an
// out-of-line definition, both built from the same parameter list; a default
// argument repeated on both is illegal in C++.
TEST_CASE(defaulted_param, "defaulted parameter", int n = 0)
{
    CHECK(n >= 0);
}

#endif
