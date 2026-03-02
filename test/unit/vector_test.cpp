// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#include <inplace/vector.hpp>

#include <gtest/gtest.h>

using inplace::vector;

TEST(vector_test, TODO) {
    auto vec = vector<int, 6>{};

    vec.push_back(312);
    vec.try_push_back(123);
    vec.unchecked_push_back(814);

    vec.emplace_back(111);
    vec.try_emplace_back(111);
    vec.unchecked_emplace_back(111);

    std::ignore = vec.at(0);
    std::ignore = vec[0];

    std::ignore = vec.begin();
    std::ignore = vec.end();
    std::ignore = vec.rbegin();
    std::ignore = vec.rend();

    std::ignore = vec.size();
    std::ignore = vec.empty();

    std::ignore = vec.front();
    std::ignore = vec.back();
    std::ignore = vec.data();
}