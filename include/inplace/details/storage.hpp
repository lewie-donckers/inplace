// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#pragma once

#include <cstddef>
#include <new>

namespace inplace::details {

template <typename T, std::size_t N>
class alignas(T) storage {
public:
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, modernize-use-equals-default)
    constexpr storage() noexcept {};
    constexpr ~storage() = default;

    storage(const storage&) = delete;
    storage(storage&&) = delete;

    storage& operator=(const storage&) = delete;
    storage& operator=(storage&&) = delete;

    constexpr auto as_byte(std::size_t index) noexcept {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        return &data_[index * sizeof(T)];
    }

    constexpr auto as_type(std::size_t index) noexcept {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<T*>(as_byte(index));
    }

    constexpr auto as_launder(std::size_t index) noexcept { return std::launder(as_type(index)); }

private:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays)
    std::byte data_[N * sizeof(T)];
};

}  // namespace inplace::details
