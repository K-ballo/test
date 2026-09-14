# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# add_basic_test(<name> SOURCES <file>...)
#
# Creates a test executable linked against Eggs::Test (not Eggs::TestMain)
# and registers it with CTest as a single pass/fail test: the source must
# define its own main() and exit 0 to pass.
#
# Intended for tests of eggs/test/detail headers that must not depend on
# eggs::test's own TEST_CASE/CHECK/registry machinery, since that is exactly
# what they are testing. Labeled "basic" (not "unit").
#
# Both the executable target and the registered test are namespaced by a
# literal "detail" prefix, not by <name> itself: this is what keeps a target
# like _eggs_test.detail.context_frame.pass from ever colliding with a
# same-stemmed source file elsewhere, the way two different
# "context.pass.cpp" once did.
function(add_basic_test name)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES")
    if(ARG_KEYWORDS_MISSING_VALUES)
        message(
            SEND_ERROR
            "add_basic_test(${name}): missing values for keyword(s): ${ARG_KEYWORDS_MISSING_VALUES}"
        )
        return()
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(
            SEND_ERROR
            "add_basic_test(${name}): unrecognized arguments: ${ARG_UNPARSED_ARGUMENTS}"
        )
        return()
    endif()

    add_executable(_eggs_test.detail.${name} ${ARG_SOURCES})
    target_link_libraries(_eggs_test.detail.${name} PRIVATE Eggs::Test)
    set_target_properties(_eggs_test.detail.${name} PROPERTIES FOLDER "test")

    add_test(NAME eggs.test.detail/${name} COMMAND _eggs_test.detail.${name})
    set_tests_properties(eggs.test.detail/${name} PROPERTIES LABELS "basic")
endfunction()

# test_entry_hash/test_entry_equal satisfy std::unordered_set's
# heterogeneous lookup requirements.
add_basic_test(
    registry.test_entry.pass
    SOURCES registry.test_entry.pass.cpp support.hpp
)

# add() inserts into the shared cases() registry and returns the stored
# entry; a duplicate name is silently discarded, keeping the first entry.
add_basic_test(registry.add.pass SOURCES registry.add.pass.cpp support.hpp)

# is_parameterized<T> is true only when T::run() is not callable with no
# arguments.
add_basic_test(
    registry.is_parameterized.pass
    SOURCES registry.is_parameterized.pass.cpp support.hpp
)

# is_valid_instance_name requires a leading letter followed by letters,
# digits, '_', '.', '/', '-'.
add_basic_test(
    registry.is_valid_instance_name.pass
    SOURCES registry.is_valid_instance_name.pass.cpp support.hpp
)

# check/check_throws/check_throws_as/check_nothrow update assertions_passed
# and assertions_failed, return the right value on each path, and let
# eggs::test::detail::unwind propagate through uncaught.
add_basic_test(checks.pass SOURCES checks.pass.cpp support.hpp)

# EGGS_TEST_PP_CAT expands its arguments before pasting them together.
add_basic_test(pp.pass SOURCES pp.pass.cpp support.hpp)

# context_frame installs itself as run_state's current context on
# construction and restores the previous one on destruction, nesting
# correctly through prev.
add_basic_test(context_frame.pass SOURCES context_frame.pass.cpp support.hpp)
