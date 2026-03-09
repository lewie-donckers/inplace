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
#include <inplace/details/utilities.hpp>
#include <inplace/ranges.hpp>

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>

namespace inplace {

template <typename T, std::size_t N>
class vector {
public:
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

    constexpr vector() noexcept {};

    constexpr explicit vector(size_type count) {
        reserve(count);
        std::uninitialized_value_construct_n(data(), count);
        set_size(count);
    }

    constexpr vector(size_type count, const T& value) {
        reserve(count);
        std::uninitialized_fill_n(data(), count, value);
        set_size(count);
    }

    template <typename InputIt>
    constexpr vector(InputIt first, InputIt last) {
        if (const auto count = details::distance(first, last)) {
            reserve(count);
            std::uninitialized_copy(first, last, data());
            set_size(count);
        } else {
            while (first != last) {
                emplace_back(*first++);
            }
        }
    }

    template <details::container_compatible_range<T> R>
    constexpr vector(from_range_t, R&& range);

    constexpr vector(std::initializer_list<T> init) {
        reserve(init.size());
        std::uninitialized_copy(init.begin(), init.end(), data());
        set_size(init.size());
    }

    constexpr vector(const vector& other)
        requires std::is_trivially_copy_constructible_v<T>
    = default;
    constexpr vector(const vector& other) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        std::uninitialized_copy(other.begin(), other.end(), data());
        set_size(other.size());
    }

    constexpr vector(vector&& other) noexcept
        requires std::is_trivially_move_constructible_v<T>
    = default;
    constexpr vector(vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        std::uninitialized_move(other.begin(), other.end(), data());
        set_size(other.size());
    }

    constexpr ~vector()
        requires std::is_trivially_destructible_v<T>
    = default;
    constexpr ~vector() { clear(); }

    constexpr vector& operator=(const vector& other) {
        if (this != &other) {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    constexpr vector& operator=(vector&& other) noexcept(std::is_nothrow_move_assignable_v<T> &&
                                                         std::is_nothrow_move_constructible_v<T>) {
        if (this != &other) {
            assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
        }
        return *this;
    }

    constexpr vector& operator=(std::initializer_list<T> init) {
        reserve(init.size());
        assign(init.begin(), init.end());
    }

    constexpr void assign(size_type count, const T& value);

    template <typename InputIt>
    constexpr void assign(InputIt first, InputIt last);

    constexpr void assign(std::initializer_list<T> ilist);

    template <details::container_compatible_range<T> R>
    constexpr void assign_range(R&& rg);

    [[nodiscard]] constexpr reference at(size_type pos) {
        if (pos >= size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::vector index out-of-range"});
        }
        return operator[](pos);
    }
    [[nodiscard]] constexpr const_reference at(size_type pos) const {
        if (pos >= size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::vector index out-of-range"});
        }
        return operator[](pos);
    }

    [[nodiscard]] constexpr reference operator[](size_type pos) { return *(data() + pos); }
    [[nodiscard]] constexpr const_reference operator[](size_type pos) const { return *(data() + pos); }

    [[nodiscard]] constexpr reference front() { return *data(); }
    [[nodiscard]] constexpr const_reference front() const { return *data(); }

    [[nodiscard]] constexpr reference back() { return operator[](size_ - 1); }
    [[nodiscard]] constexpr const_reference back() const { return operator[](size_ - 1); }

    [[nodiscard]] constexpr pointer data() noexcept { return data_; }
    [[nodiscard]] constexpr const_pointer data() const noexcept { return data_; }

    [[nodiscard]] constexpr iterator begin() noexcept { return data(); }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return data(); }

    [[nodiscard]] constexpr iterator end() noexcept { return data() + size_; }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size_; }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return data() + size_; }

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return std::reverse_iterator{end()}; }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator{end()}; }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return std::reverse_iterator{end()}; }

    [[nodiscard]] constexpr reverse_iterator rend() noexcept { return std::reverse_iterator{begin()}; }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return std::reverse_iterator{begin()}; }
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return std::reverse_iterator{begin()}; }

    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

    [[nodiscard]] constexpr size_type size() const noexcept { return size_; }

    [[nodiscard]] static constexpr size_type max_size() noexcept { return N; }

    [[nodiscard]] static constexpr size_type capacity() noexcept { return N; }

    constexpr void resize(size_type count);

    constexpr void resize(size_type count, const value_type& value);

    static constexpr void reserve(size_type new_cap) {
        if (new_cap > N) {
            INPLACE_THROW_OR_ABORT(std::bad_alloc{});
        }
    }

    static constexpr void shrink_to_fit() noexcept {}

    constexpr iterator insert(const_iterator pos, const T& value);

    constexpr iterator insert(const_iterator pos, T&& value);

    constexpr iterator insert(const_iterator pos, size_type count, const T& value);

    template <class InputIt>
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last);

    constexpr iterator insert(const_iterator pos, std::initializer_list<T> ilist);

    template <details::container_compatible_range<T> R>
    constexpr iterator insert_range(const_iterator pos, R&& rg);

    template <class... Args>
    constexpr iterator emplace(const_iterator position, Args&&... args);

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
        auto result = std::construct_at(data() + size_, std::forward<Ts>(args)...);
        ++size_;
        return *result;
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
        auto result = std::construct_at(data() + size_, value);
        ++size_;
        return *result;
    }
    constexpr reference unchecked_push_back(T&& value) {
        auto result = std::construct_at(data() + size_, std::move(value));
        ++size_;
        return *result;
    }

    constexpr void pop_back() {
        std::destroy_at(data() + size_ - 1);
        --size_;
    }

    template <details::container_compatible_range<T> R>
    constexpr void append_range(R&& rg);

    template <details::container_compatible_range<T> R>
    constexpr std::ranges::borrowed_iterator_t<R> try_append_range(R&& rg);

    constexpr void clear() noexcept {
        for (auto i = std::size_t{0}; i < size_; ++i) {
            std::destroy_at(data() + i);
        }
        set_size(0);
    }

    constexpr iterator erase(const_iterator pos);

    constexpr iterator erase(const_iterator first, const_iterator last);

    constexpr void swap(vector& other) noexcept(std::is_nothrow_swappable_v<T> &&
                                                std::is_nothrow_move_constructible_v<T>);

private:
    details::optimal_size_type_t<N, alignof(T)> size_{0};
    union {
        T data_[N];
    };

    constexpr void set_size(std::size_t size) noexcept { size_ = static_cast<decltype(size_)>(size); }
};

template <typename T>
class vector<T, 0>;

// operator==, etc
// swap
// erase
// erase_if

// deduction-guides

}  // namespace inplace