// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>
#include <new>
#include <utility>

namespace eggs::test::detail {

// no_destroy<T>
//
// Holds a T with static storage duration whose destructor is never run.
// Use for function-local statics whose destruction at program exit is
// unnecessary (and would otherwise trigger -Wexit-time-destructors).
//
//   static no_destroy const holder{make_value()};
//   auto const& value = holder.get();
template <typename T>
class no_destroy
{
  public:
    no_destroy() { ::new (static_cast<void*>(&storage_)) T(); }

    explicit no_destroy(T value)
    {
        ::new (static_cast<void*>(&storage_)) T(::std::move(value));
    }

    no_destroy(no_destroy const&) = delete;
    no_destroy& operator=(no_destroy const&) = delete;

    T& get() noexcept
    {
        return *::std::launder(reinterpret_cast<T*>(&storage_));
    }

    T const& get() const noexcept
    {
        return *::std::launder(reinterpret_cast<T const*>(&storage_));
    }

  private:
    alignas(T)::std::byte storage_[sizeof(T)];
};

template <typename T>
no_destroy(T) -> no_destroy<T>;

} // namespace eggs::test::detail
