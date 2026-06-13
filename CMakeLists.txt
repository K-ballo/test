# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.24)
project(Eggs.Test DESCRIPTION "A C++23 test library" LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

# Options (two spellings: short when top-level, prefixed when a subdirectory)
if(PROJECT_IS_TOP_LEVEL)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>"
    )

    option(BUILD_EXAMPLES "Build the examples" ON)
    include(CTest) # option(BUILD_TESTING ...)
    option(ENABLE_INSTALL "Generate install rules" ON)
else()
    option(EGGS_TEST_BUILD_EXAMPLES "Build the examples" OFF)
    option(EGGS_TEST_BUILD_TESTING "Build the test suite" OFF)
    option(EGGS_TEST_ENABLE_INSTALL "Generate install rules" OFF)

    # Map prefixed options back to the canonical names used below
    set(BUILD_EXAMPLES ${EGGS_TEST_BUILD_EXAMPLES})
    set(BUILD_TESTING ${EGGS_TEST_BUILD_TESTING})
    set(ENABLE_INSTALL ${EGGS_TEST_ENABLE_INSTALL})
endif()

#
# Library target.
# Internal name _eggs_test; downstream CMake code uses the alias Eggs::Test.
#
add_library(
    _eggs_test
    STATIC
    src/lib/checks.cpp
    src/lib/cli.cpp
    src/lib/main.cpp
    src/lib/runner.cpp
)
target_sources(
    _eggs_test
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS include
            FILES
                include/eggs/test.hpp
                include/eggs/test/cli.hpp
                include/eggs/test/detail/registry.hpp
                include/eggs/test/detail/checks.hpp
                include/eggs/test/detail/noinline.hpp
                include/eggs/test/detail/run_state.hpp
    PRIVATE
        FILE_SET private_headers
            TYPE HEADERS
            BASE_DIRS include
            FILES
                include/eggs/test/detail/print.hpp
                include/eggs/test/detail/stacktrace.hpp
        FILE_SET private_src_headers
            TYPE HEADERS
            BASE_DIRS src/lib
            FILES src/lib/opts.hpp
)
set_target_properties(
    _eggs_test
    PROPERTIES
        EXPORT_NAME Test
        DEBUG_POSTFIX d
        VERIFY_INTERFACE_HEADER_SETS ${BUILD_TESTING}
)
if(MSVC)
    set_target_properties(
        _eggs_test
        PROPERTIES
            COMPILE_PDB_NAME "_eggs_test"
            COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
    )
endif()
target_compile_features(_eggs_test PUBLIC cxx_std_23)

include(Cxx23Stacktrace)
target_link_libraries(_eggs_test PUBLIC ${EGGS_CXX23_STACKTRACE_LIB})

add_library(Eggs::Test ALIAS _eggs_test)

#
# Eggs::TestMain — object library that provides main().
# Link it into any test executable instead of writing your own main.
#
add_library(_eggs_test_main STATIC src/main/test_main.cpp)
target_link_libraries(_eggs_test_main PUBLIC _eggs_test)
set_target_properties(
    _eggs_test_main
    PROPERTIES EXPORT_NAME TestMain DEBUG_POSTFIX d
)
if(MSVC)
    set_target_properties(
        _eggs_test_main
        PROPERTIES
            COMPILE_PDB_NAME "_eggs_test_main"
            COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
    )
endif()

add_library(Eggs::TestMain ALIAS _eggs_test_main)

#
# Examples
#
if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

#
# Test suite
#
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

#
# Install rules
#
if(ENABLE_INSTALL)
    include(CMakePackageConfigHelpers)

    install(
        TARGETS _eggs_test _eggs_test_main
        EXPORT EggsTestTargets
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    if(MSVC)
        install(
            FILES
                "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/_eggs_test.pdb"
                "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/_eggs_test_main.pdb"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            CONFIGURATIONS Debug RelWithDebInfo
            OPTIONAL
        )
    endif()

    install(
        EXPORT EggsTestTargets
        NAMESPACE Eggs::
        FILE eggs.test-targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/eggs.test
    )

    configure_package_config_file(
        cmake/eggs.test-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/eggs.test-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/eggs.test
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/eggs.test-config-version.cmake
        VERSION 1.0.0
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/eggs.test-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/eggs.test-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/eggs.test
    )
endif()
