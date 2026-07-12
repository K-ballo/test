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
#   --eggs-run-test-fail-regex=<regex> must NOT match (repeatable)
#   --eggs-run-test-pass-regex=<regex> must match (repeatable)
#
# Runs <exe> with <args>, echoes its output (stdout and stderr merged),
# then checks:
#   1. exit code equals <N> (0 unless overridden), or, with
#      --eggs-run-test-will-fail, is non-zero
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
set(_fail_patterns)
set(_pass_patterns)

set(_i 5)
while(_i LESS CMAKE_ARGC)
    set(_arg "${CMAKE_ARGV${_i}}")
    if(_arg STREQUAL "--eggs-run-test-will-fail")
        set(_will_fail TRUE)
    elseif(_arg MATCHES "^--eggs-run-test-exits=(.*)$")
        set(_exits "${CMAKE_MATCH_1}")
    elseif(_arg MATCHES "^--eggs-run-test-fail-regex=(.*)$")
        list(APPEND _fail_patterns "${CMAKE_MATCH_1}")
    elseif(_arg MATCHES "^--eggs-run-test-pass-regex=(.*)$")
        list(APPEND _pass_patterns "${CMAKE_MATCH_1}")
    else()
        list(APPEND _args "${_arg}")
    endif()
    math(EXPR _i "${_i} + 1")
endwhile()

if(_will_fail AND DEFINED _exits)
    message(
        FATAL_ERROR
        "--eggs-run-test-exits=<N> cannot be used together with --eggs-run-test-will-fail"
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

if(_will_fail)
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
