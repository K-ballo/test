# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Usage:
#   cmake -P test_launcher.cmake -- <exe> [<control-arg>...] [<arg>...]
#
# Recognized <control-arg>:
#   --eggs-run-test-exits=<N>          expected exit code (default 0)
#   --eggs-run-test-will-fail          expect a non-zero exit code instead
#   --eggs-run-test-will-abort         expect the process to have aborted
#                                       (SIGABRT / CRT abort()), not merely
#                                       exited non-zero
#   --eggs-run-test-fail-regex=<regex> must NOT match (repeatable)
#   --eggs-run-test-pass-regex=<regex> must match (repeatable)
#
# Runs <exe> with <args>, echoes its output (stdout and stderr merged),
# then checks:
#   1. exit code equals <N> (0 unless overridden), or, with
#      --eggs-run-test-will-fail, is non-zero, or, with
#      --eggs-run-test-will-abort, indicates the process aborted
#   2. output does not match any --eggs-run-test-fail-regex=<pattern>
#   3. output matches every --eggs-run-test-pass-regex=<pattern>

# CMAKE_ARGV0/1/2 are cmake/-P/<this-script>, CMAKE_ARGV3 is "--";
# CMAKE_ARGV4 is <exe>, the rest are control args and/or passthrough args.
if(CMAKE_ARGC LESS 5 OR NOT CMAKE_ARGV3 STREQUAL "--")
    message(
        FATAL_ERROR
        "Usage: cmake -P test_launcher.cmake -- <exe> [<control-arg>...] [<arg>...]"
    )
endif()

set(_exe "${CMAKE_ARGV4}")
set(_args)
set(_will_fail FALSE)
set(_will_abort FALSE)
set(_fail_patterns)
set(_pass_patterns)

set(_i 5)
while(_i LESS CMAKE_ARGC)
    set(_arg "${CMAKE_ARGV${_i}}")
    if(_arg STREQUAL "--eggs-run-test-will-fail")
        set(_will_fail TRUE)
    elseif(_arg STREQUAL "--eggs-run-test-will-abort")
        set(_will_abort TRUE)
    elseif(_arg MATCHES "^--eggs-run-test-exits=(.*)$")
        set(_exits "${CMAKE_MATCH_1}")
    elseif(_arg MATCHES "^--eggs-run-test-fail-regex=(.*)$")
        if(CMAKE_MATCH_1)
            list(APPEND _fail_patterns "${CMAKE_MATCH_1}")
        endif()
    elseif(_arg MATCHES "^--eggs-run-test-pass-regex=(.*)$")
        if(CMAKE_MATCH_1)
            list(APPEND _pass_patterns "${CMAKE_MATCH_1}")
        endif()
    else()
        list(APPEND _args "${_arg}")
    endif()
    math(EXPR _i "${_i} + 1")
endwhile()

if((_will_fail OR _will_abort) AND DEFINED _exits)
    message(
        FATAL_ERROR
        "--eggs-run-test-exits=<N> cannot be used together with --eggs-run-test-will-fail or --eggs-run-test-will-abort"
    )
endif()

if(_will_fail AND _will_abort)
    message(
        FATAL_ERROR
        "--eggs-run-test-will-fail and --eggs-run-test-will-abort cannot be used together"
    )
endif()

if(NOT DEFINED _exits)
    set(_exits 0)
endif()

execute_process(
    COMMAND "${_exe}" ${_args}
    OUTPUT_VARIABLE _output
    ERROR_VARIABLE _output
    ECHO_OUTPUT_VARIABLE
    ECHO_ERROR_VARIABLE
    RESULT_VARIABLE _status
)

if(_will_abort)
    if(WIN32)
        # Debug CRT abort() exits with plain code 3; Release CRT abort() goes
        # through __fastfail(FAST_FAIL_FATAL_APP_EXIT), which execute_process
        # reports as a descriptive string containing NT status 0xC0000409
        # (STATUS_STACK_BUFFER_OVERRUN), not a plain exit code.
        string(TOLOWER "${_status}" _status_lower)
        if(NOT (_status EQUAL 3 OR _status_lower MATCHES "c0000409"))
            message(SEND_ERROR "exit code ${_status} (expected abort)")
        endif()
    elseif(NOT _status MATCHES "[Aa]bort")
        message(
            SEND_ERROR
            "exit status '${_status}' (expected the process to abort)"
        )
    endif()
elseif(_will_fail)
    if(_status EQUAL 0)
        message(SEND_ERROR "exit code ${_status} (expected non-zero)")
    endif()
elseif(NOT _status EQUAL _exits)
    message(SEND_ERROR "exit code ${_status} (expected ${_exits})")
endif()

foreach(_pat IN LISTS _fail_patterns)
    if(_output MATCHES "${_pat}")
        message(SEND_ERROR "output matched FAIL_REGEX: ${_pat}")
    endif()
endforeach()

foreach(_pat IN LISTS _pass_patterns)
    if(NOT _output MATCHES "${_pat}")
        message(SEND_ERROR "output did not match PASS_REGEX: ${_pat}")
    endif()
endforeach()
