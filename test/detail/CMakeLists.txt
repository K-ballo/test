# Eggs.Test
#
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# test_entry_hash/test_entry_equal satisfy std::unordered_set's
# heterogeneous lookup requirements.
add_lib_test(
    detail/registry.test_entry.pass
    SOURCES registry.test_entry.pass.cpp
    DISCOVER
)
