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
        all_verify_interface_header_sets --config $<CONFIG>
)
set_tests_properties(
    eggs.test.verify_headers
    PROPERTIES LABELS "header" RESOURCE_LOCK cmake_build
)

# ── Verify definitions in headers ─────────────────────────────────────────--

include(AddMultipleDefinitionsTest)

add_multiple_definitions_test(eggs.test.verify_definitions Eggs::Test)
set_target_properties(eggs.test.verify_definitions PROPERTIES FOLDER "header")
set_tests_properties(
    eggs.test.verify_definitions
    PROPERTIES LABELS "header" RESOURCE_LOCK cmake_build
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
        target_link_libraries(_eggs_test.${_stem} PRIVATE Eggs::TestMain)
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
    FAIL_REGEX "FAILED: false"
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
# Non-std::exception leaked from a test case: caught and reported as "UNKNOWN EXCEPTION".
add_lib_test(
    exceptions.fail.unknown
    SOURCES exceptions.fail.cpp
    ARGS "--run=leaked_unknown_exception"
    PASS_REGEX "UNKNOWN EXCEPTION"
    EXITS 1
)

# --- cli tests -----------------------------------------------------------

add_subdirectory(cli)

# --help must print usage.
add_lib_test(
    cli.help.pass
    SOURCES check.pass.cpp
    ARGS "--help"
    PASS_REGEX
        "Usage: .*_eggs_test\\.check\\.pass.*\\[options\\].*-h, --help.*--list.*list selected test case"
    EXITS 0
)

# -h must print usage.
add_lib_test(
    cli.help_alias.pass
    SOURCES check.pass.cpp
    ARGS "-h"
    PASS_REGEX
        "Usage: .*_eggs_test\\.check\\.pass.*\\[options\\].*-h, --help.*--list.*list selected test case"
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

# --- exception tests ---------------------------------------------------------

# CHECK_THROWS / REQUIRE_THROWS pass for any thrown type (int, std::exception, custom).
add_lib_test(throws.pass SOURCES throws.pass.cpp)
# CHECK_THROWS_AS / REQUIRE_THROWS_AS pass for exact and derived exception types.
add_lib_test(throws_as.pass SOURCES throws_as.pass.cpp)
# CHECK_CATCHES_AS / REQUIRE_CATCHES_AS pass and bind the caught exception to exc.
add_lib_test(catches_as.pass SOURCES catches_as.pass.cpp)
# CHECK_NOTHROW / REQUIRE_NOTHROW pass when the expression does not throw.
add_lib_test(nothrow.pass SOURCES nothrow.pass.cpp)

# CHECK_THROWS when nothing is thrown: 'did not throw' appears.
add_lib_test(
    throws.no_throw.check.fail
    SOURCES throws.fail.cpp
    ARGS "--run=check_throws_no_throw"
    PASS_REGEX "FAILED:.*did not throw"
    EXITS 1
)
# REQUIRE_THROWS when nothing is thrown: 'did not throw' appears; execution stops.
add_lib_test(
    throws.no_throw.require.fail
    SOURCES throws.fail.cpp
    ARGS "--run=require_throws_no_throw"
    PASS_REGEX "FAILED:.*did not throw"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# REQUIRE inside CHECK_THROWS propagates out: require_failed is not swallowed as a throw.
add_lib_test(
    throws.require_propagated.fail
    SOURCES throws.fail.cpp
    ARGS "--run=check_throws_require_propagated"
    PASS_REGEX "FAILED: 1 \\+ 1 == 3"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)

# CHECK_THROWS_AS when nothing is thrown: 'did not throw' appears.
add_lib_test(
    throws_as.no_throw.check.fail
    SOURCES throws_as.fail.cpp
    ARGS "--run=check_throws_as_no_throw"
    PASS_REGEX "FAILED:.*did not throw"
    EXITS 1
)
# REQUIRE_THROWS_AS when nothing is thrown: 'did not throw' appears; execution stops.
add_lib_test(
    throws_as.no_throw.require.fail
    SOURCES throws_as.fail.cpp
    ARGS "--run=require_throws_as_no_throw"
    PASS_REGEX "FAILED:.*did not throw"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# CHECK_THROWS_AS when the wrong std exception type is thrown: type and message appear.
add_lib_test(
    throws_as.wrong_type.check.fail
    SOURCES throws_as.fail.cpp
    ARGS "--run=check_throws_as_wrong_type"
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*\"wrong\".*expected std::runtime_error"
    EXITS 1
)
# REQUIRE_THROWS_AS with wrong std exception type: type and message appear; execution stops.
add_lib_test(
    throws_as.wrong_type.require.fail
    SOURCES throws_as.fail.cpp
    ARGS "--run=require_throws_as_wrong_type"
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*\"wrong\".*expected std::runtime_error"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# CHECK_THROWS_AS when a unknown wrong type is thrown: no type name or message appears.
add_lib_test(
    throws_as.wrong_unknown.check.fail
    SOURCES throws_as.fail.cpp
    ARGS "--run=check_throws_as_wrong_unknown"
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*expected std::runtime_error"
    FAIL_REGEX ": \""
    EXITS 1
)
# REQUIRE_THROWS_AS with unknown wrong type: no type or message appear; execution stops.
add_lib_test(
    throws_as.wrong_unknown.require.fail
    SOURCES throws_as.fail.cpp
    ARGS "--run=require_throws_as_wrong_unknown"
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*expected std::runtime_error"
    FAIL_REGEX "FAILED: false;: \""
    EXITS 1
)
# REQUIRE inside CHECK_THROWS_AS propagates out: require_failed is not swallowed as a throw.
add_lib_test(
    throws_as.require_propagated.fail
    SOURCES throws_as.fail.cpp
    ARGS "--run=check_throws_as_require_propagated"
    PASS_REGEX "FAILED: 1 \\+ 1 == 3"
    FAIL_REGEX "FAILED: false;threw unexpected exception"
    EXITS 1
)

# CHECK_CATCHES_AS when nothing is thrown: 'did not throw' appears.
add_lib_test(
    catches_as.no_throw.check.fail
    SOURCES catches_as.fail.cpp
    ARGS "--run=check_catches_as_no_throw"
    PASS_REGEX "FAILED:.*did not throw"
    EXITS 1
)
# REQUIRE_CATCHES_AS when nothing is thrown: 'did not throw' appears; execution stops.
add_lib_test(
    catches_as.no_throw.require.fail
    SOURCES catches_as.fail.cpp
    ARGS "--run=require_catches_as_no_throw"
    PASS_REGEX "FAILED:.*did not throw"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# CHECK_CATCHES_AS when the wrong std exception type is thrown: type and message appear.
add_lib_test(
    catches_as.wrong_type.check.fail
    SOURCES catches_as.fail.cpp
    ARGS "--run=check_catches_as_wrong_type"
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*\"wrong\".*expected std::runtime_error"
    EXITS 1
)
# REQUIRE_CATCHES_AS with wrong std exception type: type and message appear; execution stops.
add_lib_test(
    catches_as.wrong_type.require.fail
    SOURCES catches_as.fail.cpp
    ARGS "--run=require_catches_as_wrong_type"
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*\"wrong\".*expected std::runtime_error"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)

# CHECK_NOTHROW when a std exception is thrown: 'threw unexpectedly' with message appears.
add_lib_test(
    nothrow.throws.check.fail
    SOURCES nothrow.fail.cpp
    ARGS "--run=check_nothrow_throws_std_exception"
    PASS_REGEX "FAILED:.*threw unexpectedly.*\"oops\""
    EXITS 1
)
# REQUIRE_NOTHROW when a std exception is thrown: message appears; execution stops.
add_lib_test(
    nothrow.throws.require.fail
    SOURCES nothrow.fail.cpp
    ARGS "--run=require_nothrow_throws_std_exception"
    PASS_REGEX "FAILED:.*threw unexpectedly.*\"oops\""
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# CHECK_NOTHROW when a unknown exception is thrown: 'threw unexpectedly' without type or message.
add_lib_test(
    nothrow.throws_unknown.check.fail
    SOURCES nothrow.fail.cpp
    ARGS "--run=check_nothrow_throws_unknown"
    PASS_REGEX "FAILED:.*threw unexpectedly"
    FAIL_REGEX ": \""
    EXITS 1
)
# REQUIRE_NOTHROW when a unknown exception is thrown: no type or message appear; execution stops.
add_lib_test(
    nothrow.throws_unknown.require.fail
    SOURCES nothrow.fail.cpp
    ARGS "--run=require_nothrow_throws_unknown"
    PASS_REGEX "FAILED:.*threw unexpectedly"
    FAIL_REGEX "FAILED: false;: \""
    EXITS 1
)
# REQUIRE inside CHECK_NOTHROW propagates out: require_failed is not swallowed as a throw.
add_lib_test(
    nothrow.require_propagated.fail
    SOURCES nothrow.fail.cpp
    ARGS "--run=check_nothrow_require_propagated"
    PASS_REGEX "FAILED: 1 \\+ 1 == 3"
    FAIL_REGEX "FAILED: false;threw unexpectedly"
    EXITS 1
)

# --- stacktrace tests --------------------------------------------------------

# All assertion macros called from a helper function: no eggs:: internal frames appear.
# In Debug, the helper function name appears in the stacktrace.
# The .pass test exercises the non-failure path (no stacktrace is printed).
add_lib_test(stacktrace.pass SOURCES stacktrace.pass.cpp)
if(EGGS_HAS_CXX23_STACKTRACE)
    # CHECK failure from a helper: in Debug, 'failing_check_helper' appears in stacktrace.
    add_lib_test(
        stacktrace.check.fail
        SOURCES stacktrace.fail.cpp
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_check_helper,FAILED:>"
        FAIL_REGEX "eggs::"
        EXITS 1
    )
    # REQUIRE failure from a helper: in Debug, 'failing_require_helper' appears in stacktrace.
    add_lib_test(
        stacktrace.require.fail
        SOURCES stacktrace.fail.cpp
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_require_helper,FAILED:>"
        FAIL_REGEX "eggs::"
        EXITS 1
    )
    # CHECK_THROWS failure from a helper: in Debug, 'failing_throws_helper' appears in stacktrace.
    add_lib_test(
        stacktrace.throws.fail
        SOURCES stacktrace.fail.cpp
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_throws_helper,FAILED:>"
        FAIL_REGEX "eggs::"
        EXITS 1
    )
    # CHECK_THROWS_AS failure from a helper: in Debug, 'failing_throws_as_helper' appears.
    add_lib_test(
        stacktrace.throws_as.fail
        SOURCES stacktrace.fail.cpp
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_throws_as_helper,FAILED:>"
        FAIL_REGEX "eggs::"
        EXITS 1
    )
    # CHECK_CATCHES_AS failure from a helper: in Debug, 'failing_catches_helper' appears.
    add_lib_test(
        stacktrace.catches_as.fail
        SOURCES stacktrace.fail.cpp
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_catches_helper,FAILED:>"
        FAIL_REGEX "eggs::"
        EXITS 1
    )
    # CHECK_NOTHROW failure from a helper: in Debug, 'failing_nothrow_helper' appears.
    add_lib_test(
        stacktrace.nothrow.fail
        SOURCES stacktrace.fail.cpp
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_nothrow_helper,FAILED:>"
        FAIL_REGEX "eggs::"
        EXITS 1
    )
endif()
