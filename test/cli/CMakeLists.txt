# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# parse_cli compaction: known flags consumed, unknowns preserved in argv.
add_lib_test(cli/parse_cli.pass SOURCES parse_cli.pass.cpp EXITS 0)

# print_options: bare form lists --list and --run=<test_case> with descriptions.
add_lib_test(
    cli/print_options.bare.pass
    SOURCES print_options.pass.cpp
    ARGS "--run=print_options_bare"
    PASS_REGEX "--list.*list selected test case.*--run=<test_case>"
)

# print_options: desc_col=18 wraps --run=<test_case> to next line.
add_lib_test(
    cli/print_options.desc_col.pass
    SOURCES print_options.pass.cpp
    ARGS "--run=print_options_desc_col"
    PASS_REGEX "--run=<test_case>\n"
    FAIL_REGEX "--run=<test_case> "
)
