# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# ── Verify interface header sets ────────────────────────────────────────────

add_test(
    NAME eggs.test.verify_headers
    COMMAND
        ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target
        all_verify_interface_header_sets
)

# Helper — creates a test executable and registers it with CTest.
# PASS_REGEX: if given, the test passes iff its output matches the regex
#             (exit code is ignored).  Use this for tests that are expected
#             to fail or crash and whose output must contain a specific pattern.
function(add_lib_test name)
    cmake_parse_arguments(ARG "WILL_FAIL" "PASS_REGEX" "ARGS;SOURCES" ${ARGN})

    add_executable(_eggs_test.${name} ${ARG_SOURCES})
    target_link_libraries(_eggs_test.${name} PRIVATE Eggs::Test Eggs::TestMain)

    add_test(NAME eggs.test.${name} COMMAND _eggs_test.${name} ${ARG_ARGS})

    set_tests_properties(
        eggs.test.${name}
        PROPERTIES FOLDER "test" LABELS "unit"
    )
    if(ARG_PASS_REGEX)
        set_tests_properties(
            eggs.test.${name}
            PROPERTIES PASS_REGULAR_EXPRESSION "${ARG_PASS_REGEX}"
        )
    endif()
    if(ARG_WILL_FAIL)
        set_tests_properties(eggs.test.${name} PROPERTIES WILL_FAIL TRUE)
    endif()
endfunction()

# --- registration tests ------------------------------------------------------

# Multiple TEST_CASEs in one TU all run.
add_lib_test(
    registration
    SOURCES registration.cpp
    PASS_REGEX "3 of 3 test cases passed"
)

# Test cases defined across two translation units both register and run.
add_lib_test(
    registration.multi_tu
    SOURCES registration.multi_tu.a.cpp registration.multi_tu.b.cpp
    PASS_REGEX "2 of 2 test cases passed"
)

# --- passing tests -----------------------------------------------------------

# CHECK with true expressions must exit 0.
add_lib_test(check.pass SOURCES check.pass.cpp)

# REQUIRE with true expressions must exit 0 and not stop execution.
add_lib_test(require.pass SOURCES require.pass.cpp)

# CHECK/REQUIRE called from a helper function must compile and run correctly.
# The .pass test exercises the non-failure path (no stacktrace printed).
# The .check.fail/.require.fail tests exercise the failure path, verifying that
# the specific helper function name appears in the printed stacktrace frames.
add_lib_test(stacktrace.pass SOURCES stacktrace.pass.cpp)
if(EGGS_HAS_CXX23_STACKTRACE)
    add_lib_test(
        stacktrace.check.fail
        SOURCES stacktrace.check.fail.cpp
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_check_helper,FAILED:>"
    )
    add_lib_test(
        stacktrace.require.fail
        SOURCES stacktrace.require.fail.cpp
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_require_helper,FAILED:>"
    )
endif()

# --- failing tests -----------------------------------------------------------

# CHECK with a false expression must cause the test case to be reported as
# failed, and the stringified expression must appear in the output.
add_lib_test(
    check.fail
    SOURCES check.fail.cpp
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
)

# REQUIRE with a false expression must fail the test case and stop executing
# the rest of it; the stringified expression must appear in the output.
add_lib_test(
    require.fail
    SOURCES require.fail.cpp
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
)

# An exception leaked from a test case must be caught and reported as a failure.
add_lib_test(
    exceptions
    SOURCES exceptions.fail.cpp
    PASS_REGEX "EXCEPTION: deliberate failure"
)

# --- cli tests -----------------------------------------------------------

# --help must print usage.
add_lib_test(
    cli.help.pass
    SOURCES check.pass.cpp
    ARGS "--help"
    PASS_REGEX "Usage: <test-executable> \\[options\\]"
)

# -h must print usage.
add_lib_test(
    cli.help_alias.pass
    SOURCES check.pass.cpp
    ARGS "-h"
    PASS_REGEX "Usage: <test-executable> \\[options\\]"
)

# --unknown must print unknown argument.
add_lib_test(
    cli.unknown.fail
    SOURCES check.pass.cpp
    ARGS "--unknown"
    PASS_REGEX "unknown argument '--unknown'"
)

# -u must print unknown argument.
add_lib_test(
    cli.unknown_alias.fail
    SOURCES check.pass.cpp
    ARGS "-u"
    PASS_REGEX "unknown argument '-u'"
)

# --list must print exactly the registered test case names, one per line.
string(
    CONCAT _list_regex
    "^"
    "registration_(one|two|three)\n"
    "registration_(one|two|three)\n"
    "registration_(one|two|three)\n?$"
)
add_lib_test(
    cli.list.pass
    SOURCES registration.cpp
    ARGS "--list"
    PASS_REGEX "${_list_regex}"
)
