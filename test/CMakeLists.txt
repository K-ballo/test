# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# -- Verify interface header sets --------------------------------------------

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

# -- Verify definitions in headers -------------------------------------------

include(AddMultipleDefinitionsTest)

add_multiple_definitions_test(eggs.test.verify_definitions Eggs::Test)
set_target_properties(eggs.test.verify_definitions PROPERTIES FOLDER "header")
set_tests_properties(
    eggs.test.verify_definitions
    PROPERTIES LABELS "header" RESOURCE_LOCK cmake_build
)

include(EggsTest)

# add_lib_test(<name> SOURCES <file>...
#               [ARGS <arg>...] [RUN <name>...] [DISCOVER]
#               [EXITS <code>] [WILL_ABORT]
#               [FAIL_REGEX <pattern>...] [PASS_REGEX <pattern>...])
# add_lib_test(<name> SOURCES <file>... COMPILE_FAIL <macro>...
#               [FAIL_REGEX <pattern>...] [PASS_REGEX <pattern>...])
#
# Creates a test executable and registers it with CTest.
#
# SOURCES are the source files for the test executable.  RUN is test case names
# to select, each passed as --run=<name>.  ARGS are extra command-line
# arguments passed to the test executable.
#
# COMPILE_FAIL: the test passes when the source fails to compile.  Its value
# list is passed to the source as preprocessor definitions.  Cannot be combined
# with ARGS, DISCOVER, EXITS, RUN, or WILL_ABORT.  DISCOVER registers one
# CTest test per TEST_CASE (discovered at test time).  EXITS is the expected
# exit code (defaults to 0).  WILL_ABORT expects the process to aborte; cannot
# be combined with EXITS or DISCOVER.  FAIL_REGEX: the test fails if its output
# matches any of these patterns.  PASS_REGEX: the test passes only if its
# output matches all of these patterns.
function(add_lib_test name)
    cmake_parse_arguments(
        PARSE_ARGV 1
        ARG
        "DISCOVER;WILL_ABORT"
        "EXITS"
        "ARGS;COMPILE_FAIL;FAIL_REGEX;PASS_REGEX;RUN;SOURCES"
    )
    if("COMPILE_FAIL" IN_LIST ARG_KEYWORDS_MISSING_VALUES)
        set(ARG_COMPILE_FAIL "")
        list(REMOVE_ITEM ARG_KEYWORDS_MISSING_VALUES "COMPILE_FAIL")
    endif()
    if(
        DEFINED ARG_COMPILE_FAIL
        AND (
            DEFINED ARG_ARGS
            OR ARG_DISCOVER
            OR DEFINED ARG_EXITS
            OR DEFINED ARG_RUN
            OR ARG_WILL_ABORT
        )
    )
        message(
            SEND_ERROR
            "add_lib_test(${name}): COMPILE_FAIL cannot be combined with ARGS, DISCOVER, EXITS, RUN, or WILL_ABORT"
        )
        return()
    endif()
    if(ARG_KEYWORDS_MISSING_VALUES)
        message(
            SEND_ERROR
            "add_lib_test(${name}): missing values for keyword(s): ${ARG_KEYWORDS_MISSING_VALUES}"
        )
        return()
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(
            SEND_ERROR
            "add_lib_test(${name}): unrecognized arguments: ${ARG_UNPARSED_ARGUMENTS}"
        )
        return()
    endif()
    if(
        DEFINED ARG_EXITS
        AND (NOT ARG_EXITS MATCHES "^[0-9]+$" OR ARG_EXITS GREATER 255)
    )
        message(
            SEND_ERROR
            "add_lib_test(${name}): EXITS must be in the range 0-255 (got '${ARG_EXITS}')"
        )
        return()
    endif()
    if(ARG_WILL_ABORT AND (DEFINED ARG_EXITS OR ARG_DISCOVER))
        message(
            SEND_ERROR
            "add_lib_test(${name}): WILL_ABORT cannot be combined with EXITS or DISCOVER"
        )
        return()
    endif()

    list(LENGTH ARG_SOURCES _n)
    if(_n EQUAL 1 AND NOT DEFINED ARG_COMPILE_FAIL)
        cmake_path(GET ARG_SOURCES STEM LAST_ONLY _stem)
    else()
        set(_stem ${name})
    endif()

    foreach(_case_name IN LISTS ARG_RUN)
        list(APPEND ARG_ARGS "--run=${_case_name}")
    endforeach()

    set(_test_launcher
        ${CMAKE_COMMAND}
        -P
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/test_launcher.cmake
        --
    )

    if(NOT TARGET _eggs_test.${_stem})
        add_executable(_eggs_test.${_stem} ${ARG_SOURCES})
        target_link_libraries(_eggs_test.${_stem} PRIVATE Eggs::TestMain)
        set_target_properties(
            _eggs_test.${_stem}
            PROPERTIES FOLDER "test" TEST_LAUNCHER "${_test_launcher}"
        )
    endif()

    # test_launcher.cmake picks these control args.
    set(_control_args)
    foreach(_pat IN LISTS ARG_FAIL_REGEX)
        list(APPEND _control_args "--eggs-run-test-fail-regex=${_pat}")
    endforeach()
    foreach(_pat IN LISTS ARG_PASS_REGEX)
        list(APPEND _control_args "--eggs-run-test-pass-regex=${_pat}")
    endforeach()

    if(DEFINED ARG_COMPILE_FAIL)
        target_compile_definitions(
            _eggs_test.${_stem}
            PRIVATE ${ARG_COMPILE_FAIL}
        )
        set_target_properties(
            _eggs_test.${_stem}
            PROPERTIES EXCLUDE_FROM_ALL TRUE COMPILE_WARNING_AS_ERROR OFF
        )

        add_test(
            NAME eggs.test.${name}
            COMMAND
                ${_test_launcher} ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
                --target _eggs_test.${_stem} --config $<CONFIG> ${_control_args}
                --eggs-run-test-will-fail
        )
        set_tests_properties(
            eggs.test.${name}
            PROPERTIES LABELS "unit" RESOURCE_LOCK cmake_build
        )
        return()
    endif()

    if(DEFINED ARG_EXITS)
        list(APPEND _control_args "--eggs-run-test-exits=${ARG_EXITS}")
    endif()
    if(ARG_WILL_ABORT)
        list(APPEND _control_args "--eggs-run-test-will-abort")
    endif()

    if(ARG_DISCOVER)
        eggs_discover_tests(
            _eggs_test.${_stem}
            TEST_PREFIX "eggs.test.${name}/"
            EXTRA_ARGS ${_control_args} --non-interactive ${ARG_ARGS}
            PROPERTIES LABELS unit
        )
    else()
        add_test(
            NAME eggs.test.${name}
            COMMAND
                _eggs_test.${_stem} ${_control_args} --non-interactive
                ${ARG_ARGS}
        )
        set_tests_properties(eggs.test.${name} PROPERTIES LABELS "unit")
    endif()
endfunction()

# --- registration tests ------------------------------------------------------

# Multiple TEST_CASEs in one TU all run.
add_lib_test(
    registration
    SOURCES registration.cpp
    PASS_REGEX "3 test cases: 3 passed \\(100%\\)"
    EXITS 0
)

# Same executable, discovered at test time: one CTest test per TEST_CASE.
add_lib_test(
    registration.discover
    SOURCES registration.cpp
    DISCOVER
    FAIL_REGEX "test cases:"
)

# Test cases defined across two translation units both register and run.
add_lib_test(
    registration.multi_tu
    SOURCES registration.multi_tu.a.cpp registration.multi_tu.b.cpp
    PASS_REGEX "2 test cases: 2 passed \\(100%\\)"
    EXITS 0
)

# The [ RUN ] line for a test case shows its name, description, and source
# location.
add_lib_test(
    registration.info.pass
    SOURCES registration.cpp
    RUN registration_one
    PASS_REGEX
        "\\[ RUN  \\] registration_one -- first test case registers  .*registration\\.cpp:13."
    EXITS 0
)

# --- summary tests ------------------------------------------------------------

# A single-case run must not print the test-cases summary.
add_lib_test(
    summary.single_case.pass
    SOURCES summary.per_case.cpp
    RUN per_case_pass
    PASS_REGEX "per_case_pass"
    FAIL_REGEX "test cases:"
    EXITS 0
)

# The test-cases summary reports percentages and lists only the failing case(s).
add_lib_test(
    summary.mixed.fail
    SOURCES summary.mixed.cpp
    RUN mixed_pass_one mixed_fail mixed_pass_two
    PASS_REGEX
        "3 test cases: 2 passed \\(66%\\) \\| 1 failed \\(34%\\):\n- mixed_fail"
    FAIL_REGEX "- mixed_pass_one" "- mixed_pass_two"
    EXITS 1
)

# The test-cases summary omits the "| 0 failed (0%)" clause on 100% success.
add_lib_test(
    summary.full_pass.pass
    SOURCES summary.full_pass.cpp
    PASS_REGEX "3 test cases: 3 passed \\(100%\\)"
    FAIL_REGEX "\\| .*failed"
    EXITS 0
)

# The assertions summary reports percentages and failed count.
add_lib_test(
    summary.per_case.mixed.fail
    SOURCES summary.mixed.cpp
    RUN check_mixed
    PASS_REGEX
        "check_mixed -- 3 assertions: 2 passed \\(66%\\) \\| 1 failed \\(34%\\)"
    EXITS 1
)

# The assertions summary shows both counts with zero passing assertions.
add_lib_test(
    summary.per_case.fail
    SOURCES summary.per_case.cpp
    RUN per_case_fail
    PASS_REGEX
        "\\[ FAIL \\] per_case_fail -- 1 assertions: 0 passed \\(0%\\) \\| 1 failed \\(100%\\)"
    EXITS 1
)

# The assertions summary omits the "| 0 failed (0%)" clause on 100% success.
add_lib_test(
    summary.per_case.pass
    SOURCES summary.per_case.cpp
    RUN per_case_pass
    PASS_REGEX "\\[ PASS \\] per_case_pass -- 1 assertions: 1 passed \\(100%\\)"
    EXITS 0
)

# A test case with no assertions at all shows "0 assertions" with no
# passed/failed percentage clause.
add_lib_test(
    summary.per_case.zero.pass
    SOURCES summary.zero_assertions.cpp
    RUN zero_assertions_pass
    PASS_REGEX "\\[ PASS \\] zero_assertions_pass -- 0 assertions"
    FAIL_REGEX "0 assertions:"
    EXITS 0
)
add_lib_test(
    summary.per_case.zero.fail
    SOURCES summary.zero_assertions.cpp
    RUN zero_assertions_fail
    PASS_REGEX "\\[ FAIL \\] zero_assertions_fail -- 0 assertions"
    FAIL_REGEX "0 assertions:"
    EXITS 1
)

# --- passing tests -----------------------------------------------------------

# CHECK with true expressions must exit 0.
add_lib_test(check.pass SOURCES check.pass.cpp DISCOVER EXITS 0)

# REQUIRE with true expressions must exit 0 and not stop execution.
add_lib_test(require.pass SOURCES require.pass.cpp DISCOVER EXITS 0)

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
    RUN leaked_std_exception
    PASS_REGEX "EXCEPTION: deliberate failure"
    EXITS 1
)
# Non-std::exception leaked from a test case: caught and reported as "UNKNOWN EXCEPTION".
add_lib_test(
    exceptions.fail.unknown
    SOURCES exceptions.fail.cpp
    RUN leaked_unknown_exception
    PASS_REGEX "UNKNOWN EXCEPTION"
    EXITS 1
)
# Exception leaked from CHECK: caught and reported.
add_lib_test(
    exceptions.fail.check
    SOURCES exceptions.fail.cpp
    RUN leaked_exception_check
    PASS_REGEX "UNKNOWN EXCEPTION"
    EXITS 1
)
# Exception leaked from REQUIRE: caught and reported.
add_lib_test(
    exceptions.fail.require
    SOURCES exceptions.fail.cpp
    RUN leaked_exception_require
    PASS_REGEX "UNKNOWN EXCEPTION"
    EXITS 1
)

# --- run_state guard tests ----------------------------------------------------

# CHECK with no active run_state must abort.
add_lib_test(
    check.null_state.will_abort
    SOURCES check.null_state.will_abort.cpp
    WILL_ABORT
)

# REQUIRE with no active run_state must abort.
add_lib_test(
    require.null_state.will_abort
    SOURCES require.null_state.will_abort.cpp
    WILL_ABORT
)

# --- cli tests -----------------------------------------------------------

add_subdirectory(cli)

# --help must print usage.
add_lib_test(
    cli.help.pass
    SOURCES check.pass.cpp
    ARGS "--help"
    PASS_REGEX
        "Usage: .*_eggs_test\\.check\\.pass.*\\[options\\]"
        "-h, --help"
        "--list.*list selected test case"
    EXITS 0
)

# -h must print usage.
add_lib_test(
    cli.help_alias.pass
    SOURCES check.pass.cpp
    ARGS "-h"
    PASS_REGEX
        "Usage: .*_eggs_test\\.check\\.pass.*\\[options\\]"
        "-h, --help"
        "--list.*list selected test case"
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

# --exit-code=0 on a failing suite must override the exit code to 0.
add_lib_test(
    cli.exit_code.pass
    SOURCES check.fail.cpp
    RUN check_false
    ARGS "--exit-code=0"
    PASS_REGEX
        "FAILED: 1 \\+ 2 == 4"
        "\\[ FAIL \\] check_false -- 1 assertions: 0 passed \\(0%\\) \\| 1 failed \\(100%\\)"
    EXITS 0
)

# --exit-code=1 on a passing suite must override the exit code to 1.
add_lib_test(
    cli.exit_code.fail
    SOURCES check.pass.cpp
    RUN check_bool_true
    ARGS "--exit-code=1"
    PASS_REGEX
        "\\[ PASS \\] check_bool_true -- 3 assertions: 3 passed \\(100%\\)"
    EXITS 1
)

# --exit-code=<non-integer> must print an error and exit 1.
add_lib_test(
    cli.exit_code.invalid.fail
    SOURCES check.pass.cpp
    ARGS "--exit-code=oops"
    PASS_REGEX "invalid exit code 'oops'"
    EXITS 1
)

# --exit-code=<integer with trailing junk> must print an error and exit 1.
add_lib_test(
    cli.exit_code.trailing_junk.fail
    SOURCES check.pass.cpp
    ARGS "--exit-code=42abc"
    PASS_REGEX "invalid exit code '42abc'"
    EXITS 1
)

# --non-interactive must be accepted.
add_lib_test(
    cli.non_interactive.pass
    SOURCES check.pass.cpp
    RUN check_bool_true
    ARGS "--non-interactive"
    EXITS 0
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
    RUN registration_one
    ARGS "--list"
    PASS_REGEX "^registration_one\n?$"
    FAIL_REGEX "registration_two" "registration_three"
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
    RUN registration_three registration_one
    ARGS "--list"
    PASS_REGEX "${_list_run_order_regex}"
    FAIL_REGEX "registration_two"
)

# --run=<name> must not run other test cases.
add_lib_test(
    cli.run.pass
    SOURCES registration.cpp
    RUN registration_one
    PASS_REGEX "registration_one"
    FAIL_REGEX "registration_two" "registration_three"
    EXITS 0
)

# --run=<name> --run=<name> must warn and run the case only once: a second
# [ RUN ] block for it must not appear.
string(
    CONCAT _run_dedup_fail_regex
    "\\[ RUN  \\] registration_one[^\n]*"
    "(\n[^\n]*)*\n"
    "\\[ RUN  \\] registration_one"
)
add_lib_test(
    cli.run.dedup.pass
    SOURCES registration.cpp
    RUN registration_one registration_one
    PASS_REGEX "warning: duplicate test case 'registration_one'"
    FAIL_REGEX "${_run_dedup_fail_regex}"
    EXITS 0
)

# --run=<a> --run=<b> must run cases in the given order.
string(
    CONCAT _run_order_regex
    "\\[ RUN  \\] registration_three[^\n]*"
    "(\n[^\n]*)+\n"
    "\\[ RUN  \\] registration_one"
)
add_lib_test(
    cli.run.order.pass
    SOURCES registration.cpp
    RUN registration_three registration_one
    PASS_REGEX "${_run_order_regex}"
    FAIL_REGEX "registration_two"
)

# --run=<unknown> must print an error.
add_lib_test(
    cli.run.unknown.fail
    SOURCES registration.cpp
    RUN unknown
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
    RUN unknown_a unknown_b
    PASS_REGEX "${_run_unknown_multi_regex}"
    FAIL_REGEX "\\[ RUN  \\]"
    EXITS 1
)

# --- exception tests ---------------------------------------------------------

# CHECK_THROWS / REQUIRE_THROWS pass for any thrown type (int, std::exception, custom).
add_lib_test(throws.pass SOURCES throws.pass.cpp DISCOVER)
# CHECK_THROWS_AS / REQUIRE_THROWS_AS pass for exact and derived exception types.
add_lib_test(throws_as.pass SOURCES throws_as.pass.cpp DISCOVER)
# CHECK_CATCHES_AS / REQUIRE_CATCHES_AS pass and bind the caught exception to exc.
add_lib_test(catches_as.pass SOURCES catches_as.pass.cpp DISCOVER)
# CHECK_NOTHROW / REQUIRE_NOTHROW pass when the expression does not throw.
add_lib_test(nothrow.pass SOURCES nothrow.pass.cpp DISCOVER)

# CHECK_THROWS when nothing is thrown: 'did not throw' appears.
add_lib_test(
    throws.no_throw.check.fail
    SOURCES throws.fail.cpp
    RUN check_throws_no_throw
    PASS_REGEX "FAILED:.*did not throw"
    EXITS 1
)
# REQUIRE_THROWS when nothing is thrown: 'did not throw' appears; execution stops.
add_lib_test(
    throws.no_throw.require.fail
    SOURCES throws.fail.cpp
    RUN require_throws_no_throw
    PASS_REGEX "FAILED:.*did not throw"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# REQUIRE inside CHECK_THROWS propagates out: unwind is not swallowed as a throw.
add_lib_test(
    throws.require_propagated.check.fail
    SOURCES throws.fail.cpp
    RUN check_throws_require_propagated
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    FAIL_REGEX "FAILED: false" "$<$<CONFIG:Debug>:check_throws<>"
    EXITS 1
)
# REQUIRE inside REQUIRE_THROWS propagates out: unwind is not swallowed as a throw.
add_lib_test(
    throws.require_propagated.require.fail
    SOURCES throws.fail.cpp
    RUN require_throws_require_propagated
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    FAIL_REGEX "FAILED: false" "$<$<CONFIG:Debug>:check_throws<>"
    EXITS 1
)
# CHECK_THROWS rejects noexcept expressions.
add_lib_test(
    throws.noexcept.check.compile_fail
    SOURCES throws.compile_fail.cpp
    COMPILE_FAIL CHECK_THROWS_NOEXCEPT_COMPILE_FAIL
    PASS_REGEX "CHECK_THROWS\\(1 \\+ 1\\): expression is noexcept"
)
# REQUIRE_THROWS rejects noexcept expressions.
add_lib_test(
    throws.noexcept.require.compile_fail
    SOURCES throws.compile_fail.cpp
    COMPILE_FAIL REQUIRE_THROWS_NOEXCEPT_COMPILE_FAIL
    PASS_REGEX "CHECK_THROWS\\(1 \\+ 1\\): expression is noexcept"
)

# CHECK_THROWS_AS when nothing is thrown: 'did not throw' appears.
add_lib_test(
    throws_as.no_throw.check.fail
    SOURCES throws_as.fail.cpp
    RUN check_throws_as_no_throw
    PASS_REGEX "FAILED:.*did not throw"
    EXITS 1
)
# REQUIRE_THROWS_AS when nothing is thrown: 'did not throw' appears; execution stops.
add_lib_test(
    throws_as.no_throw.require.fail
    SOURCES throws_as.fail.cpp
    RUN require_throws_as_no_throw
    PASS_REGEX "FAILED:.*did not throw"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# CHECK_THROWS_AS when the wrong std::exception type is thrown: type and message appear.
add_lib_test(
    throws_as.wrong_type.check.fail
    SOURCES throws_as.fail.cpp
    RUN check_throws_as_wrong_type
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*\"wrong\".*expected std::runtime_error"
    EXITS 1
)
# REQUIRE_THROWS_AS when the wrong std::exception type is thrown: type and message appear; execution stops.
add_lib_test(
    throws_as.wrong_type.require.fail
    SOURCES throws_as.fail.cpp
    RUN require_throws_as_wrong_type
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*\"wrong\".*expected std::runtime_error"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# CHECK_THROWS_AS when an unknown wrong type is thrown: no type name or message appears.
add_lib_test(
    throws_as.wrong_unknown.check.fail
    SOURCES throws_as.fail.cpp
    RUN check_throws_as_wrong_unknown
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*expected std::runtime_error"
    FAIL_REGEX ": \""
    EXITS 1
)
# REQUIRE_THROWS_AS when an unknown wrong type is thrown: no type or message appear; execution stops.
add_lib_test(
    throws_as.wrong_unknown.require.fail
    SOURCES throws_as.fail.cpp
    RUN require_throws_as_wrong_unknown
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*expected std::runtime_error"
    FAIL_REGEX "FAILED: false" ": \""
    EXITS 1
)
# REQUIRE inside CHECK_THROWS_AS propagates out: unwind is not swallowed as a throw.
add_lib_test(
    throws_as.require_propagated.check.fail
    SOURCES throws_as.fail.cpp
    RUN check_throws_as_require_propagated
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    FAIL_REGEX
        "FAILED: false"
        "threw unexpected exception"
        "$<$<CONFIG:Debug>:check_throws_as<>"
    EXITS 1
)
# REQUIRE inside REQUIRE_THROWS_AS propagates out: unwind is not swallowed as a throw.
add_lib_test(
    throws_as.require_propagated.require.fail
    SOURCES throws_as.fail.cpp
    RUN require_throws_as_require_propagated
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    FAIL_REGEX
        "FAILED: false"
        "threw unexpected exception"
        "$<$<CONFIG:Debug>:check_throws_as<>"
    EXITS 1
)
# CHECK_THROWS_AS rejects noexcept expressions.
add_lib_test(
    throws_as.noexcept.check.compile_fail
    SOURCES throws_as.compile_fail.cpp
    COMPILE_FAIL CHECK_THROWS_AS_NOEXCEPT_COMPILE_FAIL
    PASS_REGEX
        "CHECK_THROWS_AS\\(std::runtime_error, 1 \\+ 1\\): expression is noexcept"
)
# REQUIRE_THROWS_AS rejects noexcept expressions.
add_lib_test(
    throws_as.noexcept.require.compile_fail
    SOURCES throws_as.compile_fail.cpp
    COMPILE_FAIL REQUIRE_THROWS_AS_NOEXCEPT_COMPILE_FAIL
    PASS_REGEX
        "CHECK_THROWS_AS\\(std::runtime_error, 1 \\+ 1\\): expression is noexcept"
)
# CHECK_THROWS_AS(unwind, ...) must not compile: unwind cannot be caught.
add_lib_test(
    throws_as.unwind.check.compile_fail
    SOURCES throws_as.compile_fail.cpp
    COMPILE_FAIL CHECK_THROWS_AS_UNWIND
    PASS_REGEX "cannot catch eggs::test::detail::unwind"
)
# REQUIRE_THROWS_AS(unwind, ...) must not compile: unwind cannot be caught.
add_lib_test(
    throws_as.unwind.require.compile_fail
    SOURCES throws_as.compile_fail.cpp
    COMPILE_FAIL REQUIRE_THROWS_AS_UNWIND
    PASS_REGEX "cannot catch eggs::test::detail::unwind"
)

# CHECK_CATCHES_AS when nothing is thrown: 'did not throw' appears.
add_lib_test(
    catches_as.no_throw.check.fail
    SOURCES catches_as.fail.cpp
    RUN check_catches_as_no_throw
    PASS_REGEX "FAILED:.*did not throw"
    EXITS 1
)
# REQUIRE_CATCHES_AS when nothing is thrown: 'did not throw' appears; execution stops.
add_lib_test(
    catches_as.no_throw.require.fail
    SOURCES catches_as.fail.cpp
    RUN require_catches_as_no_throw
    PASS_REGEX "FAILED:.*did not throw"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# CHECK_CATCHES_AS when the wrong std::exception type is thrown: type and message appear.
add_lib_test(
    catches_as.wrong_type.check.fail
    SOURCES catches_as.fail.cpp
    RUN check_catches_as_wrong_type
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*\"wrong\".*expected std::runtime_error"
    EXITS 1
)
# REQUIRE_CATCHES_AS when the wrong std::exception type is thrown: type and message appear; execution stops.
add_lib_test(
    catches_as.wrong_type.require.fail
    SOURCES catches_as.fail.cpp
    RUN require_catches_as_wrong_type
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*\"wrong\".*expected std::runtime_error"
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# CHECK_CATCHES_AS when an unknown wrong type is thrown: no type name or message appears.
add_lib_test(
    catches_as.wrong_unknown.check.fail
    SOURCES catches_as.fail.cpp
    RUN check_catches_as_wrong_unknown
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*expected std::runtime_error"
    FAIL_REGEX ": \""
    EXITS 1
)
# REQUIRE_CATCHES_AS when an unknown wrong type is thrown: no type or message appear; execution stops.
add_lib_test(
    catches_as.wrong_unknown.require.fail
    SOURCES catches_as.fail.cpp
    RUN require_catches_as_wrong_unknown
    PASS_REGEX
        "FAILED:.*threw unexpected exception.*expected std::runtime_error"
    FAIL_REGEX "FAILED: false" ": \""
    EXITS 1
)
# REQUIRE inside CHECK_CATCHES_AS propagates out: unwind is not swallowed as a throw.
add_lib_test(
    catches_as.require_propagated.check.fail
    SOURCES catches_as.fail.cpp
    RUN check_catches_as_require_propagated
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    FAIL_REGEX "FAILED: false" "$<$<CONFIG:Debug>:check_throws_as<>"
    EXITS 1
)
# REQUIRE inside REQUIRE_CATCHES_AS propagates out: unwind is not swallowed as a throw.
add_lib_test(
    catches_as.require_propagated.require.fail
    SOURCES catches_as.fail.cpp
    RUN require_catches_as_require_propagated
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    FAIL_REGEX "FAILED: false" "$<$<CONFIG:Debug>:check_throws_as<>"
    EXITS 1
)
# CHECK_CATCHES_AS rejects noexcept expressions.
add_lib_test(
    catches_as.noexcept.check.compile_fail
    SOURCES catches_as.compile_fail.cpp
    COMPILE_FAIL CHECK_CATCHES_AS_NOEXCEPT_COMPILE_FAIL
    PASS_REGEX
        "CHECK_THROWS_AS\\(std::runtime_error, 1 \\+ 1\\): expression is noexcept"
)
# REQUIRE_CATCHES_AS rejects noexcept expressions.
add_lib_test(
    catches_as.noexcept.require.compile_fail
    SOURCES catches_as.compile_fail.cpp
    COMPILE_FAIL REQUIRE_CATCHES_AS_NOEXCEPT_COMPILE_FAIL
    PASS_REGEX
        "CHECK_THROWS_AS\\(std::runtime_error, 1 \\+ 1\\): expression is noexcept"
)
# CHECK_CATCHES_AS(unwind, ...) must not compile: unwind cannot be caught.
add_lib_test(
    catches_as.unwind.check.compile_fail
    SOURCES catches_as.compile_fail.cpp
    COMPILE_FAIL CHECK_CATCHES_AS_UNWIND
    PASS_REGEX "cannot catch eggs::test::detail::unwind"
)
# REQUIRE_CATCHES_AS(unwind, ...) must not compile: unwind cannot be caught.
add_lib_test(
    catches_as.unwind.require.compile_fail
    SOURCES catches_as.compile_fail.cpp
    COMPILE_FAIL REQUIRE_CATCHES_AS_UNWIND
    PASS_REGEX "cannot catch eggs::test::detail::unwind"
)

# CHECK_NOTHROW when a std::exception is thrown: 'threw unexpectedly' with message appears.
add_lib_test(
    nothrow.throws.check.fail
    SOURCES nothrow.fail.cpp
    RUN check_nothrow_throws_std_exception
    PASS_REGEX "FAILED:.*threw unexpectedly.*\"oops\""
    EXITS 1
)
# REQUIRE_NOTHROW when a std::exception is thrown: message appears; execution stops.
add_lib_test(
    nothrow.throws.require.fail
    SOURCES nothrow.fail.cpp
    RUN require_nothrow_throws_std_exception
    PASS_REGEX "FAILED:.*threw unexpectedly.*\"oops\""
    FAIL_REGEX "FAILED: false"
    EXITS 1
)
# CHECK_NOTHROW when an unknown exception is thrown: 'threw unexpectedly' without type or message.
add_lib_test(
    nothrow.throws_unknown.check.fail
    SOURCES nothrow.fail.cpp
    RUN check_nothrow_throws_unknown
    PASS_REGEX "FAILED:.*threw unexpectedly"
    FAIL_REGEX ": \""
    EXITS 1
)
# REQUIRE_NOTHROW when an unknown exception is thrown: no type or message appear; execution stops.
add_lib_test(
    nothrow.throws_unknown.require.fail
    SOURCES nothrow.fail.cpp
    RUN require_nothrow_throws_unknown
    PASS_REGEX "FAILED:.*threw unexpectedly"
    FAIL_REGEX "FAILED: false" ": \""
    EXITS 1
)
# REQUIRE inside CHECK_NOTHROW propagates out: unwind is not swallowed as a throw.
add_lib_test(
    nothrow.require_propagated.check.fail
    SOURCES nothrow.fail.cpp
    RUN check_nothrow_require_propagated
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    FAIL_REGEX
        "FAILED: false"
        "threw unexpectedly"
        "$<$<CONFIG:Debug>:check_nothrow<>"
    EXITS 1
)
# REQUIRE inside REQUIRE_NOTHROW propagates out: unwind is not swallowed as a throw.
add_lib_test(
    nothrow.require_propagated.require.fail
    SOURCES nothrow.fail.cpp
    RUN require_nothrow_require_propagated
    PASS_REGEX "FAILED: 1 \\+ 2 == 4"
    FAIL_REGEX
        "FAILED: false"
        "threw unexpectedly"
        "$<$<CONFIG:Debug>:check_nothrow<>"
    EXITS 1
)

# --- stacktrace tests --------------------------------------------------------

# All assertion macros called from a helper function: no eggs:: internal frames appear.
# .one_level.fail: in Debug, the immediate helper name appears in the stacktrace.
# .two_levels.fail: in Debug, the outer helper name appears (all user frames shown).
# The .pass test exercises the non-failure path (no stacktrace is printed).
add_lib_test(stacktrace.pass SOURCES stacktrace.pass.cpp DISCOVER)
if(EGGS_HAS_CXX23_STACKTRACE)
    # CHECK failure directly in TEST_CASE body: no stacktrace must appear.
    add_lib_test(
        stacktrace.check.zero_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_check_zero_levels
        PASS_REGEX "FAILED:"
        FAIL_REGEX "#1 "
        EXITS 1
    )
    # CHECK failure one level deep: in Debug, 'failing_check_helper' appears in stacktrace.
    add_lib_test(
        stacktrace.check.one_level.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_check_one_level
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_check_helper,FAILED:>"
        FAIL_REGEX "check_failed"
        EXITS 1
    )
    # CHECK failure two levels deep: in Debug, 'outer_check_helper' appears in stacktrace.
    add_lib_test(
        stacktrace.check.two_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_check_two_levels
        PASS_REGEX "$<IF:$<CONFIG:Debug>,outer_check_helper,FAILED:>"
        FAIL_REGEX "check_failed"
        EXITS 1
    )

    # REQUIRE failure directly in TEST_CASE body: no stacktrace must appear.
    add_lib_test(
        stacktrace.require.zero_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_require_zero_levels
        PASS_REGEX "FAILED:"
        FAIL_REGEX "#1 "
        EXITS 1
    )
    # REQUIRE failure one level deep: in Debug, 'failing_require_helper' appears in stacktrace.
    add_lib_test(
        stacktrace.require.one_level.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_require_one_level
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_require_helper,FAILED:>"
        FAIL_REGEX "check_failed"
        EXITS 1
    )
    # REQUIRE failure two levels deep: in Debug, 'outer_require_helper' appears in stacktrace.
    add_lib_test(
        stacktrace.require.two_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_require_two_levels
        PASS_REGEX "$<IF:$<CONFIG:Debug>,outer_require_helper,FAILED:>"
        FAIL_REGEX "check_failed"
        EXITS 1
    )

    # CHECK_THROWS failure directly in TEST_CASE body: no stacktrace must appear.
    add_lib_test(
        stacktrace.throws.zero_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_throws_zero_levels
        PASS_REGEX "FAILED:"
        FAIL_REGEX "#1 "
        EXITS 1
    )
    # CHECK_THROWS failure one level deep: in Debug, 'failing_throws_helper' appears in stacktrace.
    add_lib_test(
        stacktrace.throws.one_level.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_throws_one_level
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_throws_helper,FAILED:>"
        FAIL_REGEX "check_throws<"
        EXITS 1
    )
    # CHECK_THROWS failure two levels deep: in Debug, 'outer_throws_helper' appears in stacktrace.
    add_lib_test(
        stacktrace.throws.two_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_throws_two_levels
        PASS_REGEX "$<IF:$<CONFIG:Debug>,outer_throws_helper,FAILED:>"
        FAIL_REGEX "check_throws<"
        EXITS 1
    )

    # CHECK_THROWS_AS failure directly in TEST_CASE body: no stacktrace must appear.
    add_lib_test(
        stacktrace.throws_as.zero_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_throws_as_zero_levels
        PASS_REGEX "FAILED:"
        FAIL_REGEX "#1 "
        EXITS 1
    )
    # CHECK_THROWS_AS failure one level deep: in Debug, 'failing_throws_as_helper' appears.
    add_lib_test(
        stacktrace.throws_as.one_level.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_throws_as_one_level
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_throws_as_helper,FAILED:>"
        FAIL_REGEX "check_throws_as<"
        EXITS 1
    )
    # CHECK_THROWS_AS failure two levels deep: in Debug, 'outer_throws_as_helper' appears.
    add_lib_test(
        stacktrace.throws_as.two_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_throws_as_two_levels
        PASS_REGEX "$<IF:$<CONFIG:Debug>,outer_throws_as_helper,FAILED:>"
        FAIL_REGEX "check_throws_as<"
        EXITS 1
    )

    # CHECK_CATCHES_AS failure directly in TEST_CASE body: no stacktrace must appear.
    add_lib_test(
        stacktrace.catches_as.zero_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_catches_zero_levels
        PASS_REGEX "FAILED:"
        FAIL_REGEX "#1 "
        EXITS 1
    )
    # CHECK_CATCHES_AS failure one level deep: in Debug, 'failing_catches_helper' appears.
    add_lib_test(
        stacktrace.catches_as.one_level.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_catches_one_level
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_catches_helper,FAILED:>"
        FAIL_REGEX "check_throws_as<"
        EXITS 1
    )
    # CHECK_CATCHES_AS failure two levels deep: in Debug, 'outer_catches_helper' appears.
    add_lib_test(
        stacktrace.catches_as.two_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_catches_two_levels
        PASS_REGEX "$<IF:$<CONFIG:Debug>,outer_catches_helper,FAILED:>"
        FAIL_REGEX "check_throws_as<"
        EXITS 1
    )

    # CHECK_NOTHROW failure directly in TEST_CASE body: no stacktrace must appear.
    add_lib_test(
        stacktrace.nothrow.zero_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_nothrow_zero_levels
        PASS_REGEX "FAILED:"
        FAIL_REGEX "#1 "
        EXITS 1
    )
    # CHECK_NOTHROW failure one level deep: in Debug, 'failing_nothrow_helper' appears.
    add_lib_test(
        stacktrace.nothrow.one_level.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_nothrow_one_level
        PASS_REGEX "$<IF:$<CONFIG:Debug>,failing_nothrow_helper,FAILED:>"
        FAIL_REGEX "check_nothrow<"
        EXITS 1
    )
    # CHECK_NOTHROW failure two levels deep: in Debug, 'outer_nothrow_helper' appears.
    add_lib_test(
        stacktrace.nothrow.two_levels.fail
        SOURCES stacktrace.fail.cpp
        RUN stacktrace_nothrow_two_levels
        PASS_REGEX "$<IF:$<CONFIG:Debug>,outer_nothrow_helper,FAILED:>"
        FAIL_REGEX "check_nothrow<"
        EXITS 1
    )
endif()
