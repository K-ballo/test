# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# parse_cli compaction: known flags consumed, unknowns preserved in argv.
add_lib_test(cli/parse_cli.pass SOURCES parse_cli.pass.cpp DISCOVER EXITS 0)

# print_option: disp and its first desc line share a row when disp fits.
add_lib_test(
    cli/print_option.fits.pass
    SOURCES print_option.pass.cpp
    RUN print_option_fits
    PASS_REGEX "-h, --help[ ]+print this help message and exit"
)

# print_option: disp wider than desc_col wraps desc to the next line, indented
# rather than appended to the disp line.
add_lib_test(
    cli/print_option.wraps.pass
    SOURCES print_option.pass.cpp
    RUN print_option_wraps
    PASS_REGEX "--very-long-flag-name\n" "[ ]+description text"
    FAIL_REGEX "--very-long-flag-name "
)

# print_option: desc lines past the first are indented, not appended to the
# previous line.
add_lib_test(
    cli/print_option.multi_desc.pass
    SOURCES print_option.pass.cpp
    RUN print_option_multi_desc
    PASS_REGEX "flag[ ]+first line\n" "[ ]+second line"
)

# print_options: bare form lists --list and --run=<test_case> with descriptions.
add_lib_test(
    cli/print_options.bare.pass
    SOURCES print_options.pass.cpp
    RUN print_options_bare
    PASS_REGEX "--list.*list selected test case" "--run=<test_case>"
)

# print_options: ns form prefixes every flag with --<ns>:<flag>.
add_lib_test(
    cli/print_options.ns.pass
    SOURCES print_options.pass.cpp
    RUN print_options_ns
    PASS_REGEX "--ns:list.*list selected test case" "--ns:run=<test_case>"
    FAIL_REGEX "  --list"
)

# print_options: desc_col=18 wraps --run=<test_case> to next line.
add_lib_test(
    cli/print_options.desc_col.pass
    SOURCES print_options.pass.cpp
    RUN print_options_desc_col
    PASS_REGEX "--run=<test_case>\n"
    FAIL_REGEX "--run=<test_case> "
)
