// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>
#include <source_location>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace eggs::test::detail {

struct test_entry
{
    std::string_view name;
    std::string_view desc;
    void (*run)();
    std::source_location loc;
};

struct test_entry_hash
{
    using is_transparent = void;

    std::size_t operator()(test_entry const& e) const noexcept
    {
        return std::hash<std::string_view>{}(e.name);
    }

    std::size_t operator()(std::string_view name) const noexcept
    {
        return std::hash<std::string_view>{}(name);
    }
};

struct test_entry_equal
{
    using is_transparent = void;

    bool operator()(test_entry const& a, test_entry const& b) const noexcept
    {
        return a.name == b.name;
    }

    bool operator()(std::string_view name, test_entry const& e) const noexcept
    {
        return name == e.name;
    }

    bool operator()(test_entry const& e, std::string_view name) const noexcept
    {
        return e.name == name;
    }
};

struct registry
{
    using cases_type =
        std::unordered_set<test_entry, test_entry_hash, test_entry_equal>;

    static cases_type& cases();

    static void add(test_entry e) { cases().insert(e); }

    static int run(std::vector<test_entry> const& run);
};

} // namespace eggs::test::detail
