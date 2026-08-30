# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# test_entry_hash/test_entry_equal satisfy std::unordered_set's
# heterogeneous lookup requirements.
add_basic_test(
    detail/registry.test_entry.pass
    SOURCES registry.test_entry.pass.cpp
)

# is_parameterized<T> is true only when T::run() is not callable with no
# arguments.
add_basic_test(
    detail/registry.is_parameterized.pass
    SOURCES registry.is_parameterized.pass.cpp
)

# is_valid_instance_name requires a leading letter followed by letters,
# digits, '_', '.', '/', '-'.
add_basic_test(
    detail/registry.is_valid_instance_name.pass
    SOURCES registry.is_valid_instance_name.pass.cpp
)

# check/check_throws/check_throws_as/check_nothrow update assertions_passed
# and assertions_failed, return the right value on each path, and let
# eggs::test::detail::unwind propagate through uncaught.
add_basic_test(detail/checks.pass SOURCES checks.pass.cpp)

# EGGS_TEST_PP_CAT expands its arguments before pasting them together.
add_basic_test(detail/pp.pass SOURCES pp.pass.cpp)
