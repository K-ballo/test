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

#elif defined(TEST_CASE_NO_DESC_WITH_PARAMS_COMPILE_FAIL)

// TEST_CASE rejects a parameter in place of a description.
TEST_CASE(no_desc_with_params, int const& a)
{
    CHECK(a + 1 == 1 + a);
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

#elif defined(TEST_CASE_DEFAULTED_PARAM_COMPILE_FAIL)

// TEST_CASE parameters cannot be defaulted.
TEST_CASE(defaulted_param, "defaulted parameter", int n = 0)
{
    CHECK(n >= 0);
}

#elif defined(TEST_CASE_NAMESPACE_SCOPE_COMPILE_FAIL)

// TEST_CASE must be defined at global scope.
namespace test_case_namespace_scope_compile_fail {

TEST_CASE(namespace_scope, "test case nested in a namespace")
{
    CHECK(1 + 1 == 2);
}

} // namespace test_case_namespace_scope_compile_fail

#elif defined(TEST_CASE_NAMESPACE_SCOPE_SHADOW_COMPILE_FAIL)

// TEST_CASE must be defined at global scope (shadow case).
TEST_CASE(same_name, "test case defined at global scope")
{
    CHECK(1 + 1 == 2);
}

namespace test_case_namespace_scope_shadow_compile_fail {

TEST_CASE(same_name, "test case nested in a namespace, same name as above")
{
    CHECK(1 + 1 == 2);
}

} // namespace test_case_namespace_scope_shadow_compile_fail

#endif
