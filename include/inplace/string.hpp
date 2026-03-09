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

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <string_view>
#include <type_traits>

namespace inplace {

template <typename C, std::size_t N>
class basic_string {
public:
    static_assert(N >= 2, "inplace::basic_string requires at least 2 characters to store a string");

    static_assert(std::is_same_v<C, char> || std::is_same_v<C, wchar_t> || std::is_same_v<C, char8_t> ||
                      std::is_same_v<C, char16_t> || std::is_same_v<C, char32_t>,
                  "inplace::basic_string only supports standard character types");

    using value_type = C;
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

    inline static constexpr size_type npos = size_type(-1);

    constexpr basic_string() noexcept { init(); }

    constexpr basic_string(size_type count, C ch) {
        init();
        assign(count, ch);
    }

    template <typename InputIt>
    constexpr basic_string(InputIt first, InputIt last) {
        init();
        assign(first, last);
    }

    template <details::container_compatible_range<C> R>
    constexpr basic_string(from_range_t, R&& rg) {
        init();
        assign_range(std::forward<R>(rg));
    }

    constexpr basic_string(const C* s, size_type count) {
        init();
        assign(s, count);
    }

    constexpr basic_string(const C* s) {
        init();
        assign(s);
    }

    basic_string(std::nullptr_t) = delete;

    template <details::string_view_like<C> V>
    explicit constexpr basic_string(const V& v) {
        init();
        assign(v);
    }

    template <details::string_view_like<C> V>
    constexpr basic_string(const V& v, size_type pos, size_type count) {
        init();
        assign(v, pos, count);
    }

    constexpr basic_string(const basic_string& other) noexcept = default;

    constexpr basic_string(basic_string&& other) noexcept = default;

    constexpr basic_string(std::initializer_list<C> ilist) {
        init();
        assign(ilist);
    }

    constexpr ~basic_string() noexcept = default;

    constexpr basic_string& operator=(const basic_string& str) noexcept = default;

    constexpr basic_string& operator=(basic_string&& str) noexcept = default;

    constexpr basic_string& operator=(const C* s) { return assign(s); }

    constexpr basic_string& operator=(C ch) { return assign(&ch, 1); }

    constexpr basic_string& operator=(std::initializer_list<C> ilist) { return assign(ilist); }

    template <details::string_view_like<C> V>
    constexpr basic_string& operator=(const V& v) {
        return assign(v);
    }

    basic_string& operator=(std::nullptr_t) = delete;

    constexpr basic_string& assign(const basic_string& str) noexcept { return *this = str; }

    constexpr basic_string& assign(basic_string&& str) noexcept { return *this = std::move(str); }

    constexpr basic_string& assign(size_type count, C ch) {
        reserve(count);
        std::fill_n(data_, count, ch);
        set_size_and_terminate(count);
        return *this;
    }

    constexpr basic_string& assign(const C* s, size_type count) {
        reserve(count);
        std::copy_n(s, count, data_);
        set_size_and_terminate(count);
        return *this;
    }

    constexpr basic_string& assign(const C* s) {
        clear();
        while (*s != terminator) {
            push_back(*s);
            ++s;
        }
        return *this;
    }

    template <details::string_view_like<C> V>
    constexpr basic_string& assign(const V& v) {
        const auto as_view = std::basic_string_view<C>{v};
        return assign(as_view.data(), as_view.size());
    }

    template <details::string_view_like<C> V>
    constexpr basic_string& assign(const V& v, size_type pos, size_type count = npos) {
        const auto as_view = std::basic_string_view<C>{v};
        return assign(as_view.substr(pos, count));
    }

    template <typename InputIt>
    constexpr basic_string& assign(InputIt first, InputIt last) {
        if (const auto count = details::distance(first, last)) {
            reserve(count);
            std::copy(first, last, data_);
            set_size_and_terminate(count);
        } else {
            clear();
            while (first != last) {
                emplace_back(*first);
                ++first;
            }
        }
        return *this;
    }

    constexpr basic_string& assign(std::initializer_list<C> ilist) {
        const auto count = ilist.size();
        reserve(count);
        std::copy(ilist.begin(), ilist.end(), data_);
        set_size_and_terminate(count);
        return *this;
    }

    // assign_range

    [[nodiscard]] constexpr C& at(size_type pos) {
        if (pos >= size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::basic_string index out-of-range"});
        }
        return data_[pos];
    }

    [[nodiscard]] constexpr const C& at(size_type pos) const {
        if (pos >= size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::basic_string index out-of-range"});
        }
        return data_[pos];
    }

    [[nodiscard]] constexpr C& operator[](size_type pos) {
        if (pos > size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::basic_string index out-of-range"});
        }
        return data_[pos];
    }

    [[nodiscard]] constexpr const C& operator[](size_type pos) const {
        if (pos > size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::basic_string index out-of-range"});
        }
        return data_[pos];
    }

    [[nodiscard]] constexpr C& front() { return operator[](0); }

    [[nodiscard]] constexpr const C& front() const { return operator[](0); }

    [[nodiscard]] constexpr C& back() { return operator[](size_ - 1); }

    [[nodiscard]] constexpr const C& back() const { return operator[](size_ - 1); }

    [[nodiscard]] constexpr C* data() noexcept { return data_; }

    [[nodiscard]] constexpr const char* data() const noexcept { return data_; }

    [[nodiscard]] constexpr const char* c_str() const noexcept { return data_; }

    [[nodiscard]] constexpr operator std::basic_string_view<C>() const noexcept { return as_string_view(); }

    [[nodiscard]] constexpr iterator begin() noexcept { return data_; }

    [[nodiscard]] constexpr const_iterator begin() const noexcept { return data_; }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return data_; }

    [[nodiscard]] constexpr iterator end() noexcept { return data_ + size_; }

    [[nodiscard]] constexpr const_iterator end() const noexcept { return data_ + size_; }

    [[nodiscard]] constexpr const_iterator cend() const noexcept { return data_ + size_; }

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return std::reverse_iterator{end()}; }

    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator{end()}; }

    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return std::reverse_iterator{end()}; }

    [[nodiscard]] constexpr reverse_iterator rend() noexcept { return std::reverse_iterator{begin()}; }

    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return std::reverse_iterator{begin()}; }

    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return std::reverse_iterator{begin()}; }

    [[nodiscard]] constexpr bool empty() const noexcept { return size_ != 0; }

    [[nodiscard]] constexpr size_type size() const noexcept { return size_; }

    [[nodiscard]] constexpr size_type length() const noexcept { return size_; }

    [[nodiscard]] static constexpr size_type max_size() noexcept { return N - 1; }

    constexpr void reserve(size_type new_cap) {
        if (new_cap >= N) {
            INPLACE_THROW_OR_ABORT(std::length_error{"inplace::basic_string capacity exceeded"});
        }
    }

    [[nodiscard]] static constexpr size_type capacity() noexcept { return N - 1; }

    static constexpr void shrink_to_fit() noexcept {}

    constexpr void clear() noexcept { set_size_and_terminate(0); }

    // insert
    // insert_range
    // erase

    constexpr void push_back(C ch) {
        reserve(size_ + 1);
        data_[size_] = ch;
        set_size_and_terminate(size_ + 1);
    }

    constexpr void pop_back() noexcept { set_size_and_terminate(size_ - 1); }

    // append
    // append_range
    // operator+=
    // replace
    // replace_with_range
    // copy
    // resize
    // resize_and_overwrite

    constexpr void swap(basic_string& other) noexcept {
        auto temp = std::move(other);
        other = std::move(*this);
        *this = std::move(temp);
    }

    [[nodiscard]] constexpr size_type find(const basic_string& str, size_type pos = 0) const noexcept {
        return as_string_view().find(str.as_string_view(), pos);
    }

    [[nodiscard]] constexpr size_type find(const C* s, size_type pos, size_type count) const noexcept {
        return as_string_view().find(s, pos, count);
    }

    [[nodiscard]] constexpr size_type find(const C* s, size_type pos = 0) const noexcept {
        return as_string_view().find(s, pos);
    }

    [[nodiscard]] constexpr size_type find(C ch, size_type pos = 0) const noexcept {
        return as_string_view().find(ch, pos);
    }

    template <details::string_view_like<C> V>
    [[nodiscard]] constexpr size_type find(const V& v, size_type pos = 0) const
        noexcept(std::is_nothrow_convertible_v<const V&, std::basic_string_view<C>>) {
        const auto as_view = std::basic_string_view<C>{v};
        return as_string_view().find(as_view, pos);
    }

    [[nodiscard]] constexpr size_type rfind(const basic_string& str, size_type pos = npos) const noexcept {
        return as_string_view().rfind(str.as_string_view(), pos);
    }

    [[nodiscard]] constexpr size_type rfind(const C* s, size_type pos, size_type count) const noexcept {
        return as_string_view().rfind(s, pos, count);
    }

    [[nodiscard]] constexpr size_type rfind(const C* s, size_type pos = npos) const noexcept {
        return as_string_view().rfind(s, pos);
    }

    [[nodiscard]] constexpr size_type rfind(C ch, size_type pos = npos) const noexcept {
        return as_string_view().rfind(ch, pos);
    }

    template <details::string_view_like<C> V>
    [[nodiscard]] constexpr size_type rfind(const V& v, size_type pos = npos) const
        noexcept(std::is_nothrow_convertible_v<const V&, std::basic_string_view<C>>) {
        const auto as_view = std::basic_string_view<C>{v};
        return as_string_view().rfind(as_view, pos);
    }

    [[nodiscard]] constexpr size_type find_first_of(const basic_string& str, size_type pos = 0) const noexcept {
        return as_string_view().find_first_of(str.as_string_view(), pos);
    }

    [[nodiscard]] constexpr size_type find_first_of(const C* s, size_type pos, size_type count) const noexcept {
        return as_string_view().find_first_of(s, pos, count);
    }

    [[nodiscard]] constexpr size_type find_first_of(const C* s, size_type pos = 0) const noexcept {
        return as_string_view().find_first_of(s, pos);
    }

    [[nodiscard]] constexpr size_type find_first_of(C ch, size_type pos = 0) const noexcept {
        return as_string_view().find_first_of(ch, pos);
    }

    template <details::string_view_like<C> V>
    [[nodiscard]] constexpr size_type find_first_of(const V& v, size_type pos = 0) const
        noexcept(std::is_nothrow_convertible_v<const V&, std::basic_string_view<C>>) {
        const auto as_view = std::basic_string_view<C>{v};
        return as_string_view().find_first_of(as_view, pos);
    }

    [[nodiscard]] constexpr size_type find_first_not_of(const basic_string& str, size_type pos = 0) const noexcept {
        return as_string_view().find_first_not_of(str.as_string_view(), pos);
    }

    [[nodiscard]] constexpr size_type find_first_not_of(const C* s, size_type pos, size_type count) const noexcept {
        return as_string_view().find_first_not_of(s, pos, count);
    }

    [[nodiscard]] constexpr size_type find_first_not_of(const C* s, size_type pos = 0) const noexcept {
        return as_string_view().find_first_not_of(s, pos);
    }

    [[nodiscard]] constexpr size_type find_first_not_of(C ch, size_type pos = 0) const noexcept {
        return as_string_view().find_first_not_of(ch, pos);
    }

    template <details::string_view_like<C> V>
    [[nodiscard]] constexpr size_type find_first_not_of(const V& v, size_type pos = 0) const
        noexcept(std::is_nothrow_convertible_v<const V&, std::basic_string_view<C>>) {
        const auto as_view = std::basic_string_view<C>{v};
        return as_string_view().find_first_not_of(as_view, pos);
    }

    [[nodiscard]] constexpr size_type find_last_of(const basic_string& str, size_type pos = npos) const noexcept {
        return as_string_view().find_last_of(str.as_string_view(), pos);
    }

    [[nodiscard]] constexpr size_type find_last_of(const C* s, size_type pos, size_type count) const noexcept {
        return as_string_view().find_last_of(s, pos, count);
    }

    [[nodiscard]] constexpr size_type find_last_of(const C* s, size_type pos = npos) const noexcept {
        return as_string_view().find_last_of(s, pos);
    }

    [[nodiscard]] constexpr size_type find_last_of(C ch, size_type pos = npos) const noexcept {
        return as_string_view().find_last_of(ch, pos);
    }

    template <details::string_view_like<C> V>
    [[nodiscard]] constexpr size_type find_last_of(const V& v, size_type pos = npos) const
        noexcept(std::is_nothrow_convertible_v<const V&, std::basic_string_view<C>>) {
        const auto as_view = std::basic_string_view<C>{v};
        return as_string_view().find_last_of(as_view, pos);
    }

    [[nodiscard]] constexpr size_type find_last_not_of(const basic_string& str, size_type pos = npos) const noexcept {
        return as_string_view().find_last_not_of(str.as_string_view(), pos);
    }

    [[nodiscard]] constexpr size_type find_last_not_of(const C* s, size_type pos, size_type count) const noexcept {
        return as_string_view().find_last_not_of(s, pos, count);
    }

    [[nodiscard]] constexpr size_type find_last_not_of(const C* s, size_type pos = npos) const noexcept {
        return as_string_view().find_last_not_of(s, pos);
    }

    [[nodiscard]] constexpr size_type find_last_not_of(C ch, size_type pos = npos) const noexcept {
        return as_string_view().find_last_not_of(ch, pos);
    }

    template <details::string_view_like<C> V>
    [[nodiscard]] constexpr size_type find_last_not_of(const V& v, size_type pos = npos) const
        noexcept(std::is_nothrow_convertible_v<const V&, std::basic_string_view<C>>) {
        const auto as_view = std::basic_string_view<C>{v};
        return as_string_view().find_last_not_of(as_view, pos);
    }

    // compare

    [[nodiscard]] constexpr bool starts_with(std::basic_string_view<C> sv) const noexcept {
        return as_string_view().starts_with(sv);
    }

    [[nodiscard]] constexpr bool starts_with(C ch) const noexcept { return as_string_view().starts_with(ch); }

    [[nodiscard]] constexpr bool starts_with(const C* s) const noexcept { return as_string_view().starts_with(s); }

    [[nodiscard]] constexpr bool ends_with(std::basic_string_view<C> sv) const noexcept {
        return as_string_view().ends_with(sv);
    }

    [[nodiscard]] constexpr bool ends_with(C ch) const noexcept { return as_string_view().ends_with(ch); }

    [[nodiscard]] constexpr bool ends_with(const C* s) const noexcept { return as_string_view().ends_with(s); }

    [[nodiscard]] constexpr bool contains(std::basic_string_view<C> sv) const noexcept { return find(sv) != npos; }

    [[nodiscard]] constexpr bool contains(C ch) const noexcept { return find(ch) != npos; }

    [[nodiscard]] constexpr bool contains(const C* s) const noexcept { return find(s) != npos; }

    [[nodiscard]] constexpr basic_string substr(size_type pos = 0, size_type count = npos) const {
        if (pos > size_) {
            INPLACE_THROW_OR_ABORT(std::out_of_range{"inplace::basic_string index out-of-range"});
        }
        return basic_string{as_string_view().substr(pos, count)};
    }

    // non-member operator+
    // non-member operator==
    // non-member operator<=>
    // non-member swap
    // non-member erase
    // non-member erase_if

private:
    inline static constexpr auto terminator = C{};

    details::optimal_size_type_t<N, alignof(C)> size_{0};
    union {
        C data_[N];
    };

    constexpr void init() noexcept {
        if (std::is_constant_evaluated()) {
            for (auto i = size_type{0}; i < N; ++i) {
                data_[i] = terminator;
            }
        } else {
            data_[0] = terminator;
        }
    }

    constexpr void set_size_and_terminate(size_type size) noexcept {
        size_ = static_cast<decltype(size_)>(size);
        data_[size] = terminator;
    }

    [[nodiscard]] constexpr std::basic_string_view<C> as_string_view() const noexcept { return {data_, size_}; }
};

template <std::size_t S>
using string = basic_string<char, S>;

template <std::size_t S>
using wstring = basic_string<wchar_t, S>;

template <std::size_t S>
using u8string = basic_string<char8_t, S>;

template <std::size_t S>
using u16string = basic_string<char16_t, S>;

template <std::size_t S>
using u32string = basic_string<char32_t, S>;

}  // namespace inplace