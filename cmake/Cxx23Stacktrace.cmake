# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(CheckCXXSourceCompiles)
include(CMakePushCheckState)

# Tries to compile+link std::stacktrace::current() with the given library (empty = none).
# Sets the named cache variable TRUE/FALSE via check_cxx_source_compiles.
function(_eggs_try_stacktrace lib var)
    cmake_push_check_state(RESET)

    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_REQUIRED_LIBRARIES "${lib}")
    check_cxx_source_compiles(
        [[
        #include <stacktrace>
        int main() { return (int)std::stacktrace::current().size(); }
    ]]
        ${var}
    )

    cmake_pop_check_state()
endfunction()

set(EGGS_HAS_CXX23_STACKTRACE FALSE)
set(EGGS_CXX23_STACKTRACE_LIB "")

_eggs_try_stacktrace("" EGGS_CXX23_STACKTRACE_NO_LIB)
if(EGGS_CXX23_STACKTRACE_NO_LIB)
    set(EGGS_HAS_CXX23_STACKTRACE TRUE)
    set(EGGS_CXX23_STACKTRACE_LIB "")
    return()
endif()

_eggs_try_stacktrace("stdc++exp" EGGS_CXX23_STACKTRACE_EXP)
if(EGGS_CXX23_STACKTRACE_EXP)
    set(EGGS_HAS_CXX23_STACKTRACE TRUE)
    set(EGGS_CXX23_STACKTRACE_LIB "stdc++exp")
    return()
endif()

_eggs_try_stacktrace("stdc++_libbacktrace" EGGS_CXX23_STACKTRACE_LIBBACKTRACE)
if(EGGS_CXX23_STACKTRACE_LIBBACKTRACE)
    set(EGGS_HAS_CXX23_STACKTRACE TRUE)
    set(EGGS_CXX23_STACKTRACE_LIB "stdc++_libbacktrace")
    return()
endif()
