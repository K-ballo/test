# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# eggs_discover_tests(<target> [TEST_PREFIX prefix] [TEST_SUFFIX suffix]
#                      [EXTRA_ARGS args...] [WORKING_DIRECTORY dir]
#                      [PROPERTIES name1 value1...] [TEST_LIST var])
#
# Registers one CTest test per TEST_CASE built into the executable <target>,
# named "<prefix><test case name><suffix>" (both empty by default).
#
# EXTRA_ARGS are appended to every discovered test's command line, after
# --run=<test case>. WORKING_DIRECTORY defaults to CMAKE_CURRENT_BINARY_DIR.
# PROPERTIES are applied to every discovered test, in addition to the
# WORKING_DIRECTORY property always set.
#
# TEST_LIST names a variable, populated (in the CTest script scope, once
# discovery runs - see below) with the list of discovered CTest test names;
# defaults to <target>_TESTS.
#
# <target>'s TEST_LAUNCHER property, if set, prefixes both the discovery
# invocation ('<target> --list') and every discovered test's command line.
#
# Discovery happens at test time: each time ctest runs, it re-invokes
# '<target> --list' and regenerates the cached list of tests if it is out of
# date with respect to the executable (or to CMake having reconfigured).
# Newly added, renamed, or removed TEST_CASEs are therefore picked up without
# rerunning CMake.
function(eggs_discover_tests target)
    cmake_parse_arguments(
        PARSE_ARGV 1
        ARG
        ""
        "TEST_PREFIX;TEST_SUFFIX;WORKING_DIRECTORY;TEST_LIST"
        "EXTRA_ARGS;PROPERTIES"
    )
    # EXTRA_ARGS/PROPERTIES given zero values is a plain empty list.
    list(REMOVE_ITEM ARG_KEYWORDS_MISSING_VALUES EXTRA_ARGS PROPERTIES)
    if(ARG_KEYWORDS_MISSING_VALUES)
        message(
            SEND_ERROR
            "eggs_discover_tests(${target}): missing values for keyword(s): ${ARG_KEYWORDS_MISSING_VALUES}"
        )
        return()
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(
            SEND_ERROR
            "eggs_discover_tests(${target}): unrecognized arguments: ${ARG_UNPARSED_ARGUMENTS}"
        )
        return()
    endif()
    if(NOT DEFINED ARG_WORKING_DIRECTORY)
        set(ARG_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    endif()
    if(NOT DEFINED ARG_TEST_LIST)
        set(ARG_TEST_LIST ${target}_TESTS)
    endif()

    # Key generated files by a per-target call counter, in addition to target name.
    get_property(
        _call_index
        GLOBAL
        PROPERTY "_eggs_discover_tests_call_count_${target}"
    )
    if(NOT _call_index)
        set(_call_index 0)
    endif()
    math(EXPR _call_index "${_call_index} + 1")
    set_property(
        GLOBAL
        PROPERTY "_eggs_discover_tests_call_count_${target}" "${_call_index}"
    )

    set(_ctest_file_base "${CMAKE_CURRENT_BINARY_DIR}/${target}-${_call_index}")
    set(_ctest_include_file "${_ctest_file_base}_include.cmake")
    set(_ctest_tests_file "${_ctest_file_base}_tests.cmake")

    get_property(_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(_multi_config)
        set(_ctest_tests_file "${_ctest_file_base}_tests-$<CONFIG>.cmake")
    endif()

    set(_impl_args "\n")
    string(
        APPEND _impl_args
        "    EXECUTABLE [==[$<TARGET_FILE:${target}>]==]\n"
    )
    string(APPEND _impl_args "    OUTPUT [==[${_ctest_tests_file}]==]\n")
    string(
        APPEND _impl_args
        "    WORKING_DIRECTORY [==[${ARG_WORKING_DIRECTORY}]==]\n"
    )
    if(ARG_TEST_PREFIX)
        string(APPEND _impl_args "    TEST_PREFIX [==[${ARG_TEST_PREFIX}]==]\n")
    endif()
    if(ARG_TEST_SUFFIX)
        string(APPEND _impl_args "    TEST_SUFFIX [==[${ARG_TEST_SUFFIX}]==]\n")
    endif()
    if(ARG_EXTRA_ARGS)
        string(APPEND _impl_args "    EXTRA_ARGS [==[${ARG_EXTRA_ARGS}]==]\n")
    endif()
    if(ARG_PROPERTIES)
        string(APPEND _impl_args "    PROPERTIES [==[${ARG_PROPERTIES}]==]\n")
    endif()
    string(APPEND _impl_args "    TEST_LIST [==[${ARG_TEST_LIST}]==]\n")
    string(
        APPEND _impl_args
        "$<$<BOOL:$<TARGET_PROPERTY:${target},TEST_LAUNCHER>>:    TEST_LAUNCHER [==[$<TARGET_PROPERTY:${target},TEST_LAUNCHER>]==]\n>"
    )
    string(APPEND _impl_args "  ")

    # ${CMAKE_CURRENT_LIST_FILE} is intentionally left unexpanded (escaped)
    # here: it must resolve to the generated file itself when ctest later
    # include()s it, so that reconfiguring forces a fresh discovery even if
    # the executable's mtime did not change.
    string(
        CONCAT _ctest_include_content
        "if(NOT EXISTS \"$<TARGET_FILE:${target}>\")\n"
        "  add_test(${target}_NOT_BUILT ${target}_NOT_BUILT)\n"
        "  return()\n"
        "endif()\n"
        "if(NOT EXISTS \"${_ctest_tests_file}\" OR\n"
        "   NOT \"${_ctest_tests_file}\" IS_NEWER_THAN \"$<TARGET_FILE:${target}>\" OR\n"
        "   NOT \"${_ctest_tests_file}\" IS_NEWER_THAN \"\${CMAKE_CURRENT_LIST_FILE}\")\n"
        "  include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n"
        "  eggs_discover_tests_impl(${_impl_args})\n"
        "endif()\n"
        "include(\"${_ctest_tests_file}\")\n"
    )

    if(_multi_config)
        # Generator expressions above (e.g. $<TARGET_FILE:...>) resolve
        # differently per configuration, so each config needs its own file;
        # ${CTEST_CONFIGURATION_TYPE} (escaped, resolved by ctest at test
        # time) picks the right one for whichever config ctest is running.
        file(
            GENERATE OUTPUT "${_ctest_file_base}_include-$<CONFIG>.cmake"
            CONTENT "${_ctest_include_content}"
        )
        file(
            WRITE "${_ctest_include_file}"
            "include(\"${_ctest_file_base}_include-\${CTEST_CONFIGURATION_TYPE}.cmake\")\n"
        )
    else()
        file(
            GENERATE OUTPUT "${_ctest_include_file}"
            CONTENT "${_ctest_include_content}"
        )
    endif()

    set_property(
        DIRECTORY
        APPEND
        PROPERTY TEST_INCLUDE_FILES "${_ctest_include_file}"
    )
endfunction()

# Below this point: eggs_discover_tests_impl(), (re)defined via include()
# from the generated CTest include file above, then invoked immediately by
# name - this is what actually runs '<target> --list' and (re)writes the
# tests file.
function(eggs_discover_tests_impl)
    cmake_parse_arguments(
        PARSE_ARGV 0
        ARG
        ""
        "EXECUTABLE;OUTPUT;TEST_PREFIX;TEST_SUFFIX;EXTRA_ARGS;WORKING_DIRECTORY;PROPERTIES;TEST_LIST;TEST_LAUNCHER"
        ""
    )

    execute_process(
        COMMAND ${ARG_TEST_LAUNCHER} "${ARG_EXECUTABLE}" --list
        WORKING_DIRECTORY "${ARG_WORKING_DIRECTORY}"
        OUTPUT_VARIABLE _output
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _result
    )
    if(_result)
        message(
            WARNING
            "eggs_discover_tests: '${ARG_EXECUTABLE}' exited ${_result} - no tests registered"
        )
        file(WRITE "${ARG_OUTPUT}" "")
        return()
    endif()

    # Split on newlines; handle both LF and CRLF line endings.
    string(REPLACE "\r\n" "\n" _output "${_output}")
    string(REPLACE "\n" ";" _names "${_output}")

    # Bracket-quote every extra arg / property once; spliced verbatim into
    # each discovered test's generated add_test()/set_tests_properties() call
    # below.
    set(_extra_args_text "")
    foreach(_arg IN LISTS ARG_EXTRA_ARGS)
        string(APPEND _extra_args_text " [==[${_arg}]==]")
    endforeach()

    set(_properties_text "")
    foreach(_prop IN LISTS ARG_PROPERTIES)
        string(APPEND _properties_text " [==[${_prop}]==]")
    endforeach()

    set(_launcher_text "")
    foreach(_launcher_arg IN LISTS ARG_TEST_LAUNCHER)
        string(APPEND _launcher_text " [==[${_launcher_arg}]==]")
    endforeach()

    # Use bracket quoting [==[...]==] for test names and the executable path
    # so that slashes, colons, and other special characters are preserved
    # literally.
    #
    # Use the OLD positional form of add_test: CMake 4.x processes
    # TEST_INCLUDE_FILES with an internal parser that does not recognise the
    # new keyword form.
    set(_content "")
    set(_test_names)
    foreach(_name IN LISTS _names)
        string(STRIP "${_name}" _name)
        if(_name STREQUAL "")
            continue()
        endif()

        set(_full_name "${ARG_TEST_PREFIX}${_name}${ARG_TEST_SUFFIX}")

        # Positional: add_test(<name> <command> [args...])
        string(
            APPEND _content
            "add_test([==[${_full_name}]==]${_launcher_text} [==[${ARG_EXECUTABLE}]==] [==[--run=${_name}]==]${_extra_args_text})\n"
            "set_tests_properties([==[${_full_name}]==] PROPERTIES WORKING_DIRECTORY [==[${ARG_WORKING_DIRECTORY}]==]${_properties_text})\n"
        )

        list(APPEND _test_names "${_full_name}")
    endforeach()

    if(ARG_TEST_LIST)
        string(APPEND _content "set(${ARG_TEST_LIST} [==[${_test_names}]==])\n")
    endif()

    file(WRITE "${ARG_OUTPUT}" "${_content}")
endfunction()
