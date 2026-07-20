# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 4.0)
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
cmake_path(
    CONVERT "${EGGS_TEST_SOURCE_DIR}"
    TO_CMAKE_PATH_LIST EGGS_TEST_SOURCE_DIR
    NORMALIZE
)

include(FetchContent)
FetchContent_Declare(Eggs.Test SOURCE_DIR "${EGGS_TEST_SOURCE_DIR}")
FetchContent_MakeAvailable(Eggs.Test)

# -------------------------------------------------------------------------
# Verification target - links against the FetchContent target and runs.
# -------------------------------------------------------------------------
add_executable(consumer_verify main.cpp)
target_link_libraries(consumer_verify PRIVATE Eggs::Test Eggs::TestMain)

# -------------------------------------------------------------------------
# Print target properties at configure time so ctest -V shows them.
# -------------------------------------------------------------------------
function(print_target_properties target)
    foreach(
        _prop
        IN
        ITEMS
            INTERFACE_INCLUDE_DIRECTORIES
            INTERFACE_COMPILE_DEFINITIONS
            INTERFACE_COMPILE_FEATURES
            INTERFACE_LINK_LIBRARIES
    )
        get_target_property(_val ${target} ${_prop})
        message(STATUS "${target} ${_prop}: ${_val}")
    endforeach()
endfunction()

print_target_properties(Eggs::Test)
print_target_properties(Eggs::TestMain)

# -------------------------------------------------------------------------
# Confirm eggs_discover_tests() is available and works via add_subdirectory
# (FetchContent). Consumers must include(EggsTest) explicitly.
# -------------------------------------------------------------------------
enable_testing()
include(EggsTest)
eggs_discover_tests(consumer_verify)
