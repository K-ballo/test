# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(EggsTest)

# Helper - creates an example executable and, when BUILD_TESTING is also
# enabled, registers one CTest test per TEST_CASE via eggs_discover_tests().
function(add_example name)
    add_executable(eggs.test.example.${name} ${name}.cpp)
    target_link_libraries(
        eggs.test.example.${name}
        PRIVATE Eggs::Test Eggs::TestMain
    )
    set_target_properties(eggs.test.example.${name} PROPERTIES FOLDER "example")

    if(BUILD_TESTING)
        eggs_discover_tests(
            eggs.test.example.${name}
            TEST_PREFIX "eggs.test.example.${name}/"
            PROPERTIES LABELS "example"
        )
    endif()
endfunction()

add_example(basic)
add_example(exceptions)
add_example(helpers)
