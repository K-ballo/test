# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# parse_cli compaction: known flags consumed, unknowns preserved in argv.
add_lib_test(cli/parse_cli.pass SOURCES parse_cli.pass.cpp DISCOVER)

# print_option: disp/desc alignment and wrapping.
add_lib_test(cli/print_option.pass SOURCES print_option.pass.cpp DISCOVER)

# print_options: the built-in options table.
add_lib_test(cli/print_options.pass SOURCES print_options.pass.cpp DISCOVER)
