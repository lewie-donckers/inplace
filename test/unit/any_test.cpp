// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#include <inplace/any.hpp>

#include <gtest/gtest.h>

using inplace::move_only_any;

TEST(move_only_any_test, TODO) {
    auto a = move_only_any<16>{13};

    a = 42;

    a.emplace<double>(42.);
    EXPECT_TRUE(a.has_value());
#ifdef INPLACE_RTTI
    EXPECT_EQ(typeid(double), a.type());
#endif

    EXPECT_EQ(nullptr, any_cast<int>(&a));
    EXPECT_NE(nullptr, any_cast<double>(&a));

    auto b = std::move(a);

    EXPECT_NE(nullptr, any_cast<double>(&b));

    b = std::make_unique<int>(42);
}
