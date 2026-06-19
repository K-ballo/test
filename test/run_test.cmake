# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Usage:
#   cmake -P run_test.cmake -- <exits> R:<pass_regex> <nfail> [<fail_regex>...] <exe> [<args>...]
#
# Runs <exe> [<args>...], echoes its output (stdout and stderr merged), then
# checks, in order:
#   1. exit code equals <exits>
#   2. output does not match any <fail_regex> pattern
#   3. output matches <pass_regex> (if non-empty)

set(_args)
set(_found_sep FALSE)
math(EXPR _last "${CMAKE_ARGC} - 1")
foreach(_i RANGE 0 ${_last})
    if(_found_sep)
        list(APPEND _args "${CMAKE_ARGV${_i}}")
    elseif("${CMAKE_ARGV${_i}}" STREQUAL "--")
        set(_found_sep TRUE)
    endif()
endforeach()

list(POP_FRONT _args _exits _pass_regex_raw _nfail)
string(SUBSTRING "${_pass_regex_raw}" 2 -1 _pass_regex)

set(_fail_patterns)
set(_j 0)
while(_j LESS _nfail)
    list(POP_FRONT _args _pat)
    list(APPEND _fail_patterns "${_pat}")
    math(EXPR _j "${_j} + 1")
endwhile()

list(POP_FRONT _args _exe)

execute_process(
    COMMAND "${_exe}" ${_args}
    OUTPUT_VARIABLE _output
    ERROR_VARIABLE _output
    ECHO_OUTPUT_VARIABLE
    ECHO_ERROR_VARIABLE
    RESULT_VARIABLE _status
)

if(NOT _status EQUAL _exits)
    message(SEND_ERROR "exit code ${_status} (expected ${_exits})")
endif()

foreach(_pat IN LISTS _fail_patterns)
    if(_output MATCHES "${_pat}")
        message(SEND_ERROR "output matched FAIL_REGEX: ${_pat}")
    endif()
endforeach()

if(_pass_regex AND NOT _output MATCHES "${_pass_regex}")
    message(SEND_ERROR "output did not match PASS_REGEX: ${_pass_regex}")
endif()
