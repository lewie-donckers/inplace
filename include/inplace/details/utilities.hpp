// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <ranges>
#include <string_view>
#include <type_traits>

namespace inplace::details {

template <typename Iter>
constexpr auto distance(Iter first, Iter last) {
    if constexpr (std::sized_sentinel_for<Iter, Iter> || std::forward_iterator<Iter>) {
        return std::ranges::distance(first, last);
    } else if constexpr (std::derived_from<typename std::iterator_traits<Iter>::iterator_category,
                                           std::forward_iterator_tag>) {
        return std::distance(first, last);
    } else {
        return std::false_type{};
    }
}

template <typename C>
constexpr std::size_t strlen(const C* start) {
    const auto terminator = C{};
    const char* end = start;
    while (*end != terminator) {
        ++end;
    }
    return end - start;
}

template <class R, class T>
concept container_compatible_range =
    std::ranges::input_range<R> && std::convertible_to<std::ranges::range_reference_t<R>, T>;

template <typename T, typename C>
concept string_view_like =
    std::is_convertible_v<const T&, std::basic_string_view<C>> && !std::is_convertible_v<const T&, const C*>;

template <std::size_t M, std::size_t A>
struct optimal_size_type {
private:
    static consteval auto impl() {
        const auto is_candidate = [](auto c) {
            using T = decltype(c);

            const auto less_than_size_t = (std::numeric_limits<T>::max() < std::numeric_limits<std::size_t>::max());
            const auto large_enough = (M <= std::numeric_limits<T>::max());
            const auto align_allows_optimization = (A <= alignof(T));

            return less_than_size_t && large_enough && align_allows_optimization;
        };

        const auto u8 = std::uint8_t{};
        const auto u16 = std::uint16_t{};
        const auto u32 = std::uint32_t{};
        const auto u64 = std::uint64_t{};

        if constexpr (is_candidate(u8)) {
            return u8;
        } else if constexpr (is_candidate(u16)) {
            return u16;
        } else if constexpr (is_candidate(u32)) {
            return u32;
        } else if constexpr (is_candidate(u64)) {
            return u64;
        } else {
            return std::size_t{};
        }
    }

public:
    using type = decltype(impl());
};

template <std::size_t M, std::size_t A>
using optimal_size_type_t = typename optimal_size_type<M, A>::type;

}  // namespace inplace::details