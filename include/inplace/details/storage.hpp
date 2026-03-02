// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#pragma once

#include <array>
#include <cstddef>
#include <new>

namespace inplace::details {

template <typename T, std::size_t N>
class alignas(T) storage {
public:
    constexpr storage() noexcept = default;
    constexpr ~storage() = default;

    storage(const storage&) = delete;
    storage(storage&&) = delete;

    storage& operator=(const storage&) = delete;
    storage& operator=(storage&&) = delete;

    template <typename... Ts>
    constexpr T* construct_at(std::size_t index, Ts&&... args) {
        return std::construct_at(as_type(index), std::forward<Ts>(args)...);
    }

    constexpr void destroy_at(std::size_t index) { std::destroy_at(as_launder(index)); }

    constexpr T* operator[](std::size_t index) noexcept { return as_launder(index); }
    constexpr const T* operator[](std::size_t index) const noexcept { return as_launder(index); }

private:
    std::array<std::byte, N * sizeof(T)> data_;

    constexpr auto as_byte(std::size_t index) noexcept { return data_.data() + (index * sizeof(T)); }
    constexpr auto as_byte(std::size_t index) const noexcept { return data_.data() + (index * sizeof(T)); }

    constexpr auto as_type(std::size_t index) noexcept { return reinterpret_cast<T*>(as_byte(index)); }
    constexpr auto as_type(std::size_t index) const noexcept { return reinterpret_cast<const T*>(as_byte(index)); }

    constexpr auto as_launder(std::size_t index) noexcept { return std::launder(as_type(index)); }
    constexpr auto as_launder(std::size_t index) const noexcept { return std::launder(as_type(index)); }
};

}  // namespace inplace::details
