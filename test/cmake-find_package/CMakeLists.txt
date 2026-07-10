# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.31)
project(Eggs.Test-FindPackage LANGUAGES CXX)

set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
    "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
)

# -------------------------------------------------------------------------
# Find the installed package exactly as an end-user would.
# -------------------------------------------------------------------------
find_package(Eggs.Test REQUIRED)

# -------------------------------------------------------------------------
# Verification target - links against the imported target and runs.
# -------------------------------------------------------------------------
add_executable(consumer_verify main.cpp)
target_link_libraries(consumer_verify PRIVATE Eggs::Test Eggs::TestMain)

# -------------------------------------------------------------------------
# Print target properties at configure time so ctest -V shows them.
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
    message(STATUS "Eggs::TestMain ${_prop}: ${_val}")
endforeach()

enable_testing()

# -------------------------------------------------------------------------
# Confirm eggs_discover_tests() is available and works from the installed
# package. Consumers must include(EggsTest) explicitly.
# -------------------------------------------------------------------------
include(EggsTest)
eggs_discover_tests(consumer_verify)
