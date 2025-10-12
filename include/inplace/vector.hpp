// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#pragma once

#include <inplace/details/macros.hpp>
#include <inplace/details/storage.hpp>

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>

namespace inplace {

template <typename T, std::size_t N>
class vector {
public:
    // TODO static_assert that N < max(ptrdiff_t)?

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr vector() noexcept = default;
    constexpr explicit vector(size_type count);
    constexpr vector(size_type count, const T& value);
    template <typename InputIt>
    constexpr vector(InputIt first, InputIt last);
    // TODO requires C++23
    // template <typename R>  // TODO container-compatible-range
    // constexpr vector(std::from_range_t, R&& range);
    constexpr vector(const vector& other);
    constexpr vector(vector&& other) noexcept((N == 0) || std::is_nothrow_move_constructible_v<T>);
    constexpr vector(std::initializer_list<T> init);

    constexpr ~vector() {
        if (size_ > 0) {
            std::ranges::destroy_n(storage_.as_launder(0), static_cast<std::ptrdiff_t>(size_));
        }
    }

    constexpr vector& operator=(const vector& other);
    constexpr vector& operator=(vector&& other) noexcept((N == 0) || (std::is_nothrow_move_assignable_v<T> &&
                                                                      std::is_nothrow_move_constructible_v<T>));
    constexpr vector& operator=(std::initializer_list<T> init);

    // assign
    // assign_range

    [[nodiscard]] constexpr reference at(size_type pos) {
        if (pos >= size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::vector index out-of-range"});
        }
        return *storage_.as_launder(pos);
    }
    [[nodiscard]] constexpr const_reference at(size_type pos) const {
        if (pos >= size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::vector index out-of-range"});
        }
        return *storage_.as_launder(pos);
    }

    [[nodiscard]] constexpr reference operator[](size_type pos) { return *storage_.as_launder(pos); }
    [[nodiscard]] constexpr const_reference operator[](size_type pos) const { return *storage_.as_launder(pos); }

    [[nodiscard]] constexpr reference front() { return *storage_.as_launder(0); }
    [[nodiscard]] constexpr const_reference front() const { return *storage_.as_launder(0); }

    [[nodiscard]] constexpr reference back() { return *storage_.as_launder(size_ - 1); }
    [[nodiscard]] constexpr const_reference back() const { return *storage_.as_launder(size_ - 1); }

    // TODO launder is only valid if there is an object there. use as_type if empty?
    [[nodiscard]] constexpr pointer data() noexcept { return storage_.as_launder(0); }
    [[nodiscard]] constexpr const_pointer data() const noexcept { return storage_.as_launder(0); }

    // begin/cbegin
    // end/cend
    // rbegin/crbegin
    // rend/crend

    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] constexpr size_type size() const noexcept { return size_; }
    [[nodiscard]] static constexpr size_type max_size() noexcept { return N; }
    [[nodiscard]] static constexpr size_type capacity() noexcept { return N; }
    // resize
    static constexpr void reserve([[maybe_unused]] size_type new_cap) {}
    static constexpr void shrink_to_fit() noexcept {}

    // insert
    // insert_range
    // emplace
    // emplace_back
    // try_emplace_back
    // unchecked_emplace_back
    // push_back
    // try_push_back
    // unchecked_push_back
    // pop_back
    // append_range
    // try_append_range
    // clear
    // erase
    // swap

private:
    size_type size_{0};               // TODO use space efficient size
    details::storage<T, N> storage_;  // TODO storage needs to support trivial copying etc
};

// operator==, etc
// swap
// erase
// erase_if

// deduction-guides

}  // namespace inplace