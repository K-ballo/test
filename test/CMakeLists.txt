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
#
# PASS_REGEX: the test passes iff its output matches the regex; exit code is
#             ignored.
# FAIL_REGEX: the test fails if its output matches the regex; the exit code
#             still drives the pass/fail verdict (unlike PASS_REGEX).
# EXITS C:    the expected exit code.  With PASS_REGEX a companion .exit test
#             is created to verify the exit code independently.  Without
#             PASS_REGEX, WILL_FAIL is derived from C and applied to the main
#             test.  Without EXITS no exit code is checked.
function(add_lib_test name)
    cmake_parse_arguments(
        ARG
        ""
        "EXITS;FAIL_REGEX;PASS_REGEX"
        "ARGS;SOURCES"
        ${ARGN}
    )

    list(LENGTH ARG_SOURCES _n)
    if(_n EQUAL 1)
        get_filename_component(_stem ${ARG_SOURCES} NAME_WLE)
    else()
        set(_stem ${name})
    endif()

    if(NOT TARGET _eggs_test.${_stem})
        add_executable(_eggs_test.${_stem} ${ARG_SOURCES})
        target_link_libraries(
            _eggs_test.${_stem}
            PRIVATE Eggs::Test Eggs::TestMain
        )
        set_target_properties(_eggs_test.${_stem} PROPERTIES FOLDER "test")
    endif()

    add_test(NAME eggs.test.${name} COMMAND _eggs_test.${_stem} ${ARG_ARGS})
    set_tests_properties(eggs.test.${name} PROPERTIES LABELS "unit")

    if(ARG_FAIL_REGEX)
        set_tests_properties(
            eggs.test.${name}
            PROPERTIES FAIL_REGULAR_EXPRESSION "${ARG_FAIL_REGEX}"
        )
    endif()

    if(ARG_EXITS EQUAL 0)
        set(_will_fail FALSE)
    else()
        set(_will_fail TRUE)
    endif()
    if(ARG_PASS_REGEX)
        set_tests_properties(
            eggs.test.${name}
            PROPERTIES PASS_REGULAR_EXPRESSION "${ARG_PASS_REGEX}"
        )
        # PASS_REGEX ignores exit code; companion verifies it separately.
        if(DEFINED ARG_EXITS)
            add_test(
                NAME eggs.test.${name}.exit
                COMMAND _eggs_test.${_stem} ${ARG_ARGS}
            )
            set_tests_properties(
                eggs.test.${name}.exit
                PROPERTIES LABELS "unit" WILL_FAIL "${_will_fail}"
            )
        endif()
    elseif(DEFINED ARG_EXITS)
        # No PASS_REGEX: exit code drives the main test directly.
        set_tests_properties(
            eggs.test.${name}
            PROPERTIES WILL_FAIL "${_will_fail}"
        )
    endif()
endfunction()

# --- registration tests ------------------------------------------------------

# Multiple TEST_CASEs in one TU all run.
add_lib_test(
    registration
    SOURCES registration.cpp
    PASS_REGEX "3 of 3 test cases passed"
    EXITS 0
)

# Test cases defined across two translation units both register and run.
add_lib_test(
    registration.multi_tu
    SOURCES registration.multi_tu.a.cpp registration.multi_tu.b.cpp
    PASS_REGEX "2 of 2 test cases passed"
    EXITS 0
)

# --- passing tests -----------------------------------------------------------

# CHECK with true expressions must exit 0.
add_lib_test(check.pass SOURCES check.pass.cpp EXITS 0)

# REQUIRE with true expressions must exit 0 and not stop execution.
add_lib_test(require.pass SOURCES require.pass.cpp EXITS 0)

# CHECK/REQUIRE called from a helper function must compile and run correctly.
# The .pass test exercises the non-failure path (no stacktrace printed).
# The .check.fail/.require.fail tests exercise the failure path, verifying that
# the specific helper function name appears in the printed stacktrace frames.
add_lib_test(stacktrace.pass SOURCES stacktrace.pass.cpp EXITS 0)
if(EGGS_HAS_CXX23_STACKTRACE)
    add_lib_test(
        stacktrace.check.one_level.fail
        SOURCES stacktrace.fail.cpp
        ARGS "--run=stacktrace_check_one_level"
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_check_helper,FAILED:>"
        EXITS 1
    )
    add_lib_test(
        stacktrace.check.two_levels.fail
        SOURCES stacktrace.fail.cpp
        ARGS "--run=stacktrace_check_two_levels"
        PASS_REGEX "$<IF:$<CONFIG:Debug>,outer_check_helper,FAILED:>"
        EXITS 1
    )
    add_lib_test(
        stacktrace.require.one_level.fail
        SOURCES stacktrace.fail.cpp
        ARGS "--run=stacktrace_require_one_level"
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_require_helper,FAILED:>"
        EXITS 1
    )
    add_lib_test(
        stacktrace.require.two_levels.fail
        SOURCES stacktrace.fail.cpp
        ARGS "--run=stacktrace_require_two_levels"
        PASS_REGEX "$<IF:$<CONFIG:Debug>,outer_require_helper,FAILED:>"
        EXITS 1
    )
endif()

# --- failing tests -----------------------------------------------------------

# CHECK with a false expression must cause the test case to be reported as
# failed and the stringified expression must appear in the output.
add_lib_test(
    check.fail
    SOURCES check.fail.cpp
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    EXITS 1
)

# REQUIRE with a false expression must fail the test case and stop executing
# the rest of it; the stringified expression must appear in the output.
add_lib_test(
    require.fail
    SOURCES require.fail.cpp
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    EXITS 1
)

# An std::exception leaked from a test case: caught and reported as "EXCEPTION: message".
add_lib_test(
    exceptions.fail.std
    SOURCES exceptions.fail.cpp
    ARGS "--run=leaked_std_exception"
    PASS_REGEX "EXCEPTION: deliberate failure"
    EXITS 1
)
# Non-std exception leaked from a test case: caught and reported as "UNKNOWN EXCEPTION".
add_lib_test(
    exceptions.fail.unknown
    SOURCES exceptions.fail.cpp
    ARGS "--run=leaked_unknown_exception"
    PASS_REGEX "UNKNOWN EXCEPTION"
    EXITS 1
)

# --- cli tests -----------------------------------------------------------

# --help must print usage.
add_lib_test(
    cli.help.pass
    SOURCES check.pass.cpp
    ARGS "--help"
    PASS_REGEX "Usage: <test-executable> \\[options\\]"
    EXITS 0
)

# -h must print usage.
add_lib_test(
    cli.help_alias.pass
    SOURCES check.pass.cpp
    ARGS "-h"
    PASS_REGEX "Usage: <test-executable> \\[options\\]"
    EXITS 0
)

# --unknown must print unknown argument.
add_lib_test(
    cli.unknown.fail
    SOURCES check.pass.cpp
    ARGS "--unknown"
    PASS_REGEX "unknown argument '--unknown'"
    EXITS 1
)

# -u must print unknown argument.
add_lib_test(
    cli.unknown_alias.fail
    SOURCES check.pass.cpp
    ARGS "-u"
    PASS_REGEX "unknown argument '-u'"
    EXITS 1
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
    EXITS 0
)

# --list --run=<name> must list only the matching test case.
add_lib_test(
    cli.list.run.pass
    SOURCES registration.cpp
    ARGS "--list" "--run=registration_one"
    PASS_REGEX "^registration_one\n?$"
    FAIL_REGEX "registration_two;registration_three"
    EXITS 0
)

# --list --run=<a> --run=<b> must list names in --run order.
string(
    CONCAT _list_run_order_regex
    "^"
    "registration_three\n"
    "registration_one\n?$"
)
add_lib_test(
    cli.list.run.order.pass
    SOURCES registration.cpp
    ARGS "--list" "--run=registration_three" "--run=registration_one"
    PASS_REGEX "${_list_run_order_regex}"
    FAIL_REGEX "registration_two"
)

# --run=<name> must not run other test cases.
add_lib_test(
    cli.run.pass
    SOURCES registration.cpp
    ARGS "--run=registration_one"
    PASS_REGEX "registration_one"
    FAIL_REGEX "registration_two;registration_three"
    EXITS 0
)

# --run=<name> --run=<name> must warn and run the case only once.
string(
    CONCAT _run_dedup_regex
    "warning: duplicate test case 'registration_one'"
    "(\n[^\n]*)*\n"
    "1 of 1 test cases passed"
)
add_lib_test(
    cli.run.dedup.pass
    SOURCES registration.cpp
    ARGS "--run=registration_one" "--run=registration_one"
    PASS_REGEX "${_run_dedup_regex}"
    EXITS 0
)

# --run=<a> --run=<b> must run cases in the given order.
string(
    CONCAT _run_order_regex
    "\\[ RUN  \\] registration_three[^\n]*"
    "(\n[^\n]+)+\n"
    "\\[ RUN  \\] registration_one"
)
add_lib_test(
    cli.run.order.pass
    SOURCES registration.cpp
    ARGS "--run=registration_three" "--run=registration_one"
    PASS_REGEX "${_run_order_regex}"
    FAIL_REGEX "registration_two"
)

# --run=<unknown> must print an error.
add_lib_test(
    cli.run.unknown.fail
    SOURCES registration.cpp
    ARGS "--run=unknown"
    PASS_REGEX "error: unknown test case 'unknown'"
    FAIL_REGEX "\\[ RUN  \\]"
    EXITS 1
)

# --run=<unknown_a> --run=<unknown_b> must report all invalid names.
string(
    CONCAT _run_unknown_multi_regex
    "^"
    "error: unknown test case 'unknown_a'\n"
    "error: unknown test case 'unknown_b'\n?$"
)
add_lib_test(
    cli.run.unknown_multi.fail
    SOURCES registration.cpp
    ARGS "--run=unknown_a" "--run=unknown_b"
    PASS_REGEX "${_run_unknown_multi_regex}"
    FAIL_REGEX "\\[ RUN  \\]"
    EXITS 1
)
