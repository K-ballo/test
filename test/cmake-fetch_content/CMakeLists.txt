# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.25)
project(Eggs.Test-FetchContent LANGUAGES CXX)

set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
    "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
)

# -------------------------------------------------------------------------
# EGGS_TEST_SOURCE_DIR is injected so that this consumer always tests
# the local working tree, not a remote tag.
# -------------------------------------------------------------------------
if(NOT DEFINED EGGS_TEST_SOURCE_DIR)
    message(
        FATAL_ERROR
        "EGGS_TEST_SOURCE_DIR must be set. "
        "Pass -DEGGS_TEST_SOURCE_DIR=<path> to cmake."
    )
endif()
file(TO_CMAKE_PATH "${EGGS_TEST_SOURCE_DIR}" EGGS_TEST_SOURCE_DIR)

include(FetchContent)
FetchContent_Declare(Eggs.Test SOURCE_DIR "${EGGS_TEST_SOURCE_DIR}")
FetchContent_MakeAvailable(Eggs.Test)

# -------------------------------------------------------------------------
# Verification target — links against the FetchContent target and runs.
# -------------------------------------------------------------------------
add_executable(consumer_verify main.cpp)
target_link_libraries(consumer_verify PRIVATE Eggs::Test Eggs::TestMain)

# -------------------------------------------------------------------------
# Confirm that the FetchContent target exposes the same properties as the
# installed one (no accidental in-tree-only paths).
# -------------------------------------------------------------------------
foreach(
    _prop
    IN
    ITEMS
        INTERFACE_INCLUDE_DIRECTORIES
        INTERFACE_COMPILE_DEFINITIONS
        INTERFACE_COMPILE_FEATURES
        INTERFACE_LINK_LIBRARIES
)
    get_target_property(_val Eggs::TestMain ${_prop})
    message(STATUS "Eggs::TestMain (FetchContent) ${_prop}: ${_val}")
endforeach()

enable_testing()
add_test(NAME verify COMMAND consumer_verify)
