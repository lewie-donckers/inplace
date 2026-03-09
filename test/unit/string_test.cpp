// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#include <inplace/string.hpp>

#include <gtest/gtest.h>

#include <type_traits>

using inplace::string;
using inplace::u16string;
using inplace::u32string;
using inplace::u8string;
using inplace::wstring;

static_assert(std::is_trivially_copyable_v<string<16>>);

static_assert(sizeof(string<16>) == 17);
static_assert(alignof(string<16>) == 1);
static_assert(sizeof(string<1024>) == 1026);
static_assert(alignof(string<1024>) == 2);
static_assert(sizeof(string<102400>) == 102404);
static_assert(alignof(string<102400>) == 4);

static_assert(sizeof(u16string<4>) == 10);
static_assert(alignof(u16string<4>) == 2);
static_assert(sizeof(u16string<1024>) == 2050);
static_assert(alignof(u16string<1024>) == 2);
static_assert(sizeof(u16string<102400>) == 204804);
static_assert(alignof(u16string<102400>) == 4);

TEST(string_test, TODO) {
    auto str = string<16>{};
    auto str2 = string<16>{"aap"};

    str.assign("test");
    std::ignore = str.at(2);
    std::ignore = str[2];
    std::ignore = str.front();
    std::ignore = str.back();
    std::ignore = str.data();
    std::ignore = str.c_str();
    std::ignore = static_cast<std::string_view>(str);
    std::ignore = str.begin();
    std::ignore = str.cbegin();
    std::ignore = str.end();
    std::ignore = str.cend();
    std::ignore = str.rbegin();
    std::ignore = str.crbegin();
    std::ignore = str.rend();
    std::ignore = str.crend();
    std::ignore = str.empty();
    std::ignore = str.size();
    std::ignore = str.length();
    std::ignore = str.max_size();
    str.reserve(5);
    std::ignore = str.capacity();
    str.shrink_to_fit();
    str.clear();
    str.push_back('a');
    str.pop_back();
    str.swap(str2);
    std::ignore = str.find("p");
    std::ignore = str.rfind('a');
    std::ignore = str.find_first_of("mies");
    std::ignore = str.find_first_not_of("mies");
    std::ignore = str.find_last_of("mies");
    std::ignore = str.find_last_not_of("mies");
    std::ignore = str.starts_with("boom");
    std::ignore = str.ends_with("boom");
    std::ignore = str.contains("boom");
    std::ignore = str.substr(3, 5);
}

TEST(string_test, TODO_consteval) {
    []() consteval {
        auto str = string<16>{};

        str.assign("test");
        std::ignore = str.at(2);
        std::ignore = str[2];
        std::ignore = str.front();
        std::ignore = str.back();
        std::ignore = str.data();
        std::ignore = str.c_str();
        std::ignore = static_cast<std::string_view>(str);
        std::ignore = str.begin();
        std::ignore = str.cbegin();
        std::ignore = str.end();
        std::ignore = str.cend();
        std::ignore = str.rbegin();
        std::ignore = str.crbegin();
        std::ignore = str.rend();
        std::ignore = str.crend();
        std::ignore = str.empty();
        std::ignore = str.size();
        std::ignore = str.length();
        std::ignore = str.max_size();
        str.reserve(5);
        std::ignore = str.capacity();
        str.shrink_to_fit();
        str.clear();
    }();
}

TEST(string_test, TODO2) {
    static constexpr auto str = string<16>{};
    static constexpr auto str2 = str;
    std::ignore = str2;
}