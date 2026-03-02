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

    constexpr ~vector() { clear(); }

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
        return *storage_[pos];
    }
    [[nodiscard]] constexpr const_reference at(size_type pos) const {
        if (pos >= size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::vector index out-of-range"});
        }
        return *storage_[pos];
    }

    [[nodiscard]] constexpr reference operator[](size_type pos) { return *storage_[pos]; }
    [[nodiscard]] constexpr const_reference operator[](size_type pos) const { return *storage_[pos]; }

    [[nodiscard]] constexpr reference front() { return *storage_[0]; }
    [[nodiscard]] constexpr const_reference front() const { return *storage_[0]; }

    [[nodiscard]] constexpr reference back() { return *storage_[size_ - 1]; }
    [[nodiscard]] constexpr const_reference back() const { return *storage_[size_ - 1]; }

    [[nodiscard]] constexpr pointer data() noexcept { return storage_[0]; }
    [[nodiscard]] constexpr const_pointer data() const noexcept { return storage_[0]; }

    [[nodiscard]] constexpr iterator begin() noexcept { return storage_[0]; }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return storage_[0]; }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return storage_[0]; }

    [[nodiscard]] constexpr iterator end() noexcept { return storage_[size_]; }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return storage_[size_]; }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return storage_[size_]; }

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return std::reverse_iterator{storage_[size_]}; }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
        return std::reverse_iterator{storage_[size_]};
    }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
        return std::reverse_iterator{storage_[size_]};
    }

    [[nodiscard]] constexpr reverse_iterator rend() noexcept { return std::reverse_iterator{storage_[0]}; }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return std::reverse_iterator{storage_[0]}; }
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return std::reverse_iterator{storage_[0]}; }

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

    template <typename... Ts>
    constexpr reference emplace_back(Ts&&... args) {
        if (size_ == N) {
            INPLACE_THROW_OR_ABORT(std::bad_alloc{});
        }
        return unchecked_emplace_back(std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    constexpr pointer try_emplace_back(Ts&&... args) {
        if (size_ == N) {
            return nullptr;
        }
        return &unchecked_emplace_back(std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    constexpr reference unchecked_emplace_back(Ts&&... args) {
        auto& result = *storage_.construct_at(size_, std::forward<Ts>(args)...);
        ++size_;
        return result;
    }

    constexpr void push_back(const T& value) {
        if (size_ == N) {
            INPLACE_THROW_OR_ABORT(std::bad_alloc{});
        }
        unchecked_push_back(value);
    }
    constexpr void push_back(T&& value) {
        if (size_ == N) {
            INPLACE_THROW_OR_ABORT(std::bad_alloc{});
        }
        unchecked_push_back(std::move(value));
    }

    constexpr pointer try_push_back(const T& value) {
        if (size_ == N) {
            return nullptr;
        }
        return &unchecked_push_back(value);
    }
    constexpr pointer try_push_back(T&& value) {
        if (size_ == N) {
            return nullptr;
        }
        return &unchecked_push_back(std::move(value));
    }

    constexpr reference unchecked_push_back(const T& value) {
        auto& result = *storage_.construct_at(size_, value);
        ++size_;
        return result;
    }
    constexpr reference unchecked_push_back(T&& value) {
        auto& result = *storage_.construct_at(size_, std::move(value));
        ++size_;
        return result;
    }

    constexpr void pop_back() {
        storage_.destroy_at(size_ - 1);
        --size_;
    }

    // append_range
    // try_append_range

    constexpr void clear() noexcept {
        for (auto i = std::size_t{0}; i < size_; ++i) storage_.destroy_at(i);
        size_ = 0;
    }

    // erase
    // swap

private:
    size_type size_{0};
    details::storage<T, N> storage_;  // TODO storage needs to support trivial copying etc
};

// operator==, etc
// swap
// erase
// erase_if

// deduction-guides

}  // namespace inplace