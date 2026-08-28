// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <concepts>

#if defined(REGISTER_P_INVALID_NAME_COMPILE_FAIL)

TEST_CASE(invalid_name, "parameterized test", int const& n)
{
    CHECK(n == n);
}

// REGISTER_P requires an identifier as a name.
REGISTER_P(invalid name, "small", 0);

#elif defined(REGISTER_P_NONLITERAL_INSTANCE_COMPILE_FAIL)

TEST_CASE(nonliteral_instance, "parameterized test", int const& n)
{
    CHECK(n == n);
}

// REGISTER_P instance name must be a string literal.
char const* const instance = "first";

REGISTER_P(nonliteral_instance, instance, 0);

#elif defined(REGISTER_P_AUTO_REGISTERED_COMPILE_FAIL)

TEST_CASE(auto_registered, "zero-parameter test")
{
    CHECK(1 + 1 == 2);
}

// REGISTER_P cannot be used on a TEST_CASE with no parameters.
REGISTER_P(auto_registered, "second");

#elif defined(REGISTER_P_WRONG_ARITY_LESS_COMPILE_FAIL)

TEST_CASE(
    wrong_arity_less, "addition is commutative", int const& a, int const& b
)
{
    CHECK(a + b == b + a);
}

// REGISTER_P rejects registration with the wrong number of arguments.
REGISTER_P(wrong_arity_less, "too_few", 1);

#elif defined(REGISTER_P_WRONG_ARITY_MORE_COMPILE_FAIL)

TEST_CASE(
    wrong_arity_more, "addition is commutative", int const& a, int const& b
)
{
    CHECK(a + b == b + a);
}

// REGISTER_P rejects registration with the wrong number of arguments.
REGISTER_P(wrong_arity_more, "too_many", 1, 2, 3);

#elif defined(REGISTER_P_INTEGRAL_AUTO_COMPILE_FAIL)

TEST_CASE(
    integral_auto_reject,
    "std::integral auto only instantiates for integral types",
    std::integral auto n
)
{
    CHECK((n | 0) == n);
}

// REGISTER_P rejects arguments that do not satisfy the constrains.
REGISTER_P(integral_auto_reject, "double", 3.14);

#elif defined(REGISTER_P_EMPTY_INSTANCE_COMPILE_FAIL)

TEST_CASE(empty_instance, "parameterized test", int const& n)
{
    CHECK(n == n);
}

// REGISTER_P rejects an empty instance name.
REGISTER_P(empty_instance, "", 0);

#elif defined(REGISTER_P_LEADING_DIGIT_INSTANCE_COMPILE_FAIL)

TEST_CASE(leading_digit_instance, "parameterized test", int const& n)
{
    CHECK(n == n);
}

// REGISTER_P rejects an instance name starting with a digit.
REGISTER_P(leading_digit_instance, "1st", 0);

#elif defined(REGISTER_P_LEADING_SYMBOL_INSTANCE_COMPILE_FAIL)

TEST_CASE(leading_symbol_instance, "parameterized test", int const& n)
{
    CHECK(n == n);
}

// REGISTER_P rejects an instance name starting with a symbol.
REGISTER_P(leading_symbol_instance, "-first", 0);

#elif defined(REGISTER_P_INVALID_CHAR_INSTANCE_COMPILE_FAIL)

TEST_CASE(invalid_char_instance, "parameterized test", int const& n)
{
    CHECK(n == n);
}

// REGISTER_P rejects an instance name containing an invalid character.
REGISTER_P(invalid_char_instance, "two words", 0);

#endif
