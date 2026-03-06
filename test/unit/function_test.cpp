// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#include "macros.hpp"

#include <inplace/functional.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace {

using inplace::function;

int add(int arg1, int arg2) { return arg1 + arg2; }

}  // namespace

TEST(function_test, construct_default) {
    const auto func = function<void()>{};

    ASSERT_FALSE(func);
}

TEST(function_test, construct_nullptr) {
    const auto func = function<void()>{nullptr};

    ASSERT_FALSE(func);
}

TEST(function_test, construct_callable_copy) {
    const auto lambda = []() {};
    const auto func = function<void()>{lambda};

    ASSERT_TRUE(func);
}

TEST(function_test, construct_callable_move) {
    const auto func = function<void()>{[]() {}};

    ASSERT_TRUE(func);
}

TEST(function_test, construct_copy) {
    {
        const auto func1 = function<void()>{};
        const auto func2 = func1;

        ASSERT_FALSE(func2);
    }
    {
        const auto func1 = function<void()>{[]() {}};
        const auto func2 = func1;

        ASSERT_TRUE(func2);
    }
}

TEST(function_test, construct_move) {
    {
        auto func1 = function<void()>{};
        const auto func2 = std::move(func1);

        ASSERT_FALSE(func2);
    }
    {
        auto func1 = function<void()>{[]() {}};
        const auto func2 = std::move(func1);

        ASSERT_TRUE(func2);
    }
}

TEST(function_test, assign_copy) {
    {
        const auto func1 = function<void()>{[]() {}};
        auto func2 = function<void()>{};

        func2 = func1;

        ASSERT_TRUE(func2);
    }
    {
        const auto func1 = function<void()>{};
        auto func2 = function<void()>{[]() {}};

        func2 = func1;

        ASSERT_FALSE(func2);
    }
}

TEST(function_test, assign_move) {
    {
        auto func1 = function<void()>{[]() {}};
        auto func2 = function<void()>{};

        func2 = std::move(func1);

        ASSERT_TRUE(func2);
    }
    {
        auto func1 = function<void()>{};
        auto func2 = function<void()>{[]() {}};

        func2 = std::move(func1);

        ASSERT_FALSE(func2);
    }
}

TEST(function_test, assign_nullptr) {
    {
        auto func = function<void()>{[]() {}};

        func = nullptr;

        ASSERT_FALSE(func);
    }
    {
        auto func = function<void()>{};

        func = nullptr;

        ASSERT_FALSE(func);
    }
}

TEST(function_test, assign_callable_copy) {
    const auto lambda = []() {};
    auto func = function<void()>{};

    func = lambda;

    ASSERT_TRUE(func);
}

TEST(function_test, assign_callable_move) {
    auto func = function<void()>{};

    func = []() {};

    ASSERT_TRUE(func);
}

TEST(function_test, operator_bool) {
    {
        const auto func = function<void()>{};

        ASSERT_FALSE(func);
    }
    {
        const auto func = function<void()>{[]() {}};

        ASSERT_TRUE(func);
    }
}

TEST(function_test, call_operator) {
    {
        const auto func = function<void()>{};

        ASSERT_THROW_OR_ABORT(func(), inplace::bad_function_call);
    }
    {
        auto nr_calls = 0;

        {
            const auto func = function<void()>{[&nr_calls]() { ++nr_calls; }};

            ASSERT_EQ(0, nr_calls);

            func();

            ASSERT_EQ(1, nr_calls);
        }

        ASSERT_EQ(1, nr_calls);
    }
}

TEST(function_test, swap) {
    auto func1 = function<void()>{};
    auto func2 = function<void()>{[]() {}};

    func1.swap(func2);

    ASSERT_TRUE(func1);
    ASSERT_FALSE(func2);
}

TEST(function_test, non_member_swap) {
    auto func1 = function<void()>{};
    auto func2 = function<void()>{[]() {}};

    swap(func1, func2);

    ASSERT_TRUE(func1);
    ASSERT_FALSE(func2);
}

TEST(function_test, comparison) {
    {
        const auto func = function<void()>{};

        ASSERT_TRUE(func == nullptr);
        ASSERT_TRUE(nullptr == func);
        ASSERT_FALSE(func != nullptr);
        ASSERT_FALSE(nullptr != func);
    }
    {
        const auto func = function<void()>{[]() {}};

        ASSERT_FALSE(func == nullptr);
        ASSERT_FALSE(nullptr == func);
        ASSERT_TRUE(func != nullptr);
        ASSERT_TRUE(nullptr != func);
    }
}

TEST(function_test, call_using_invoke) {
    const auto lambda = [](int arg1, int arg2) { return arg1 + arg2; };

    const auto func = function<int(int, int)>{lambda};
    const auto result = std::invoke(func, 40, 2);

    ASSERT_EQ(42, result);
}

TEST(function_test, with_lambda) {
    const auto lambda = [](int arg1, int arg2) { return arg1 + arg2; };

    const auto func = function<int(int, int)>{lambda};
    const auto result = func(40, 2);

    ASSERT_EQ(42, result);
}

TEST(function_test, with_function) {
    const auto func = function<int(int, int)>{add};
    const auto result = func(40, 2);

    ASSERT_EQ(42, result);
}

TEST(function_test, with_function_object) {
    struct test_type {
        int operator()(int arg1, int arg2) const { return arg1 + arg2; }
    };

    const auto func = function<int(int, int)>{test_type{}};
    const auto result = func(40, 2);

    ASSERT_EQ(42, result);
}

TEST(function_test, with_member_function) {
    struct test_type {
        int val;

        int bar(int arg) { return val += arg; }
    };

    auto test_object = test_type{40};

    const auto func = function<int(test_type&, int)>{&test_type::bar};
    const auto result = func(test_object, 2);

    ASSERT_EQ(42, result);
}

TEST(function_test, with_static_member_function) {
    struct test_type {
        static int bar(int arg1, int arg2) { return arg1 + arg2; }
    };

    const auto func = function<int(int, int)>{test_type::bar};
    const auto result = func(40, 2);

    ASSERT_EQ(42, result);
}

TEST(function_test, with_data_member) {
    struct test_type {
        int val;
    };

    const auto func = function<int(const test_type&)>{&test_type::val};
    const auto result = func(test_type{42});

    ASSERT_EQ(42, result);
}

// TEST(function_test, with_move_only_callable) {
//     auto lambda = [cap = std::make_unique<int>(40)](int arg) { return arg + *cap; };
//     static_assert(!std::is_copy_constructible_v<decltype(lambda)> && !std::is_copy_assignable_v<decltype(lambda)>);

//     auto func1 = function<int(int)>{std::move(lambda)};

//     ASSERT_EQ(42, func1(2));

//     const auto func2 = std::move(func1);

//     ASSERT_EQ(2, func2(-38));
// }

TEST(function_test, with_move_only_return_type) {
    const auto lambda = [](int arg) { return std::make_unique<int>(arg); };

    const auto func = function<std::unique_ptr<int>(int)>{lambda};
    const auto result = func(42);

    static_assert(std::is_same_v<decltype(result), const std::unique_ptr<int>>);
    ASSERT_NE(nullptr, result);
    ASSERT_EQ(42, *result);
}

TEST(function_test, with_move_only_argument) {
    const auto lambda = [](std::unique_ptr<int> arg) { return *arg; };

    const auto func = function<int(std::unique_ptr<int>)>{lambda};
    const auto result = func(std::make_unique<int>(42));

    ASSERT_EQ(42, result);
}

TEST(function_test, with_pass_through_const_l_ref) {
    const auto input = 0;
    const auto lambda = [](const int& arg) -> const int& { return arg; };

    const auto func = function<const int&(const int&)>{lambda};
    const auto& result = func(input);

    ASSERT_EQ(&input, &result);
}

TEST(function_test, with_pass_through_l_ref) {
    auto input = 0;
    const auto lambda = [](int& arg) -> int& { return arg; };

    const auto func = function<int&(int&)>{lambda};
    auto& result = func(input);

    static_assert(std::is_same_v<decltype(result), int&>);
    ASSERT_EQ(&input, &result);
}

TEST(function_test, with_pass_through_r_ref) {
    auto input = std::make_unique<int>(0);
    const auto* ptr = input.get();
    const auto lambda = [](std::unique_ptr<int>&& arg) -> std::unique_ptr<int>&& { return std::move(arg); };

    const auto func = function<std::unique_ptr<int> && (std::unique_ptr<int>&&)>{lambda};
    const auto result = func(std::move(input));

    ASSERT_EQ(ptr, result.get());
}

TEST(function_test, with_data_conversions) {
    const auto lambda = [](float arg1, float arg2) { return arg1 + arg2; };

    const auto func = function<double(double, double)>{lambda};
    const auto result = func(40., 2.);

    ASSERT_EQ(42., result);
}

TEST(function_test, with_other_inplace_function) {
    const auto lambda = [](float arg1, float arg2) { return arg1 + arg2; };

    auto func1 = function<float(float, float)>{lambda};
    const auto func2 = function<double(double, double), sizeof(decltype(func1))>{std::move(func1)};
    const auto result = func2(40., 2.);

    ASSERT_EQ(42., result);
}

TEST(function_test, deduce_lambda) {
    {
        const auto lambda = [](int arg1, int arg2) { return arg1 + arg2; };
        const auto func = function{lambda};

        static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
    }
    {
        auto capture = 0;
        const auto lambda = [&capture](int arg) { return arg + capture; };
        const auto func = function{lambda};

        static_assert(std::is_same_v<decltype(func), const function<int(int)>>);
    }
}

TEST(function_test, deduce_function) {
    const auto func = function{add};

    static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
}

TEST(function_test, deduce_function_object) {
    {
        struct test_type {
            int operator()(int arg1, int arg2) { return arg1 + arg2; }
        };

        const auto func = function{test_type{}};

        static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) const { return arg1 + arg2; }
        };

        const auto func = function{test_type{}};

        static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) & { return arg1 + arg2; }
        };

        const auto func = function{test_type{}};

        static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) const& { return arg1 + arg2; }
        };

        const auto func = function{test_type{}};

        static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) noexcept { return arg1 + arg2; }
        };

        const auto func = function{test_type{}};

        static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) const noexcept { return arg1 + arg2; }
        };

        const auto func = function{test_type{}};

        static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) & noexcept { return arg1 + arg2; }
        };

        const auto func = function{test_type{}};

        static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) const& noexcept { return arg1 + arg2; }
        };

        const auto func = function{test_type{}};

        static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
    }
}

TEST(function_test, deduce_static_member_function) {
    struct test_type {
        static int bar(int arg1, int arg2) { return arg1 + arg2; }
    };

    const auto func = function{&test_type::bar};

    static_assert(std::is_same_v<decltype(func), const function<int(int, int)>>);
}

TEST(function_test, size_requirements) {
    static constexpr auto pointer_size = sizeof(void*);

    {
        static_assert(std::is_constructible_v<function<int(int, int), pointer_size>, decltype(add)>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) { return arg1 + arg2; }
        };
        static_assert(sizeof(test_type) == 1);
        static_assert(alignof(test_type) == 1);

        static_assert(std::is_constructible_v<function<int(int, int), pointer_size>, test_type>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) { return arg1 + arg2; }

            std::uint32_t val{};
        };
        static_assert(sizeof(test_type) == 4);
        static_assert(alignof(test_type) == 4);

        static_assert(std::is_constructible_v<function<int(int, int), pointer_size>, test_type>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) { return arg1 + arg2; }

            void* val{};
        };
        static_assert(sizeof(test_type) == pointer_size);
        static_assert(alignof(test_type) == pointer_size);

        static_assert(std::is_constructible_v<function<int(int, int), pointer_size>, test_type>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) { return arg1 + arg2; }

            void* val[2]{};
        };
        static_assert(sizeof(test_type) == 2 * pointer_size);
        static_assert(alignof(test_type) == pointer_size);

        static_assert(!std::is_constructible_v<function<int(int, int), (2 * pointer_size) - 1>, test_type>);
        static_assert(std::is_constructible_v<function<int(int, int), 2 * pointer_size>, test_type>);
        static_assert(std::is_constructible_v<function<int(int, int)>, test_type>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) { return arg1 + arg2; }

            alignas(2 * pointer_size) void* val[2]{};
        };
        static_assert(sizeof(test_type) == 2 * pointer_size);
        static_assert(alignof(test_type) == 2 * pointer_size);

        static_assert(std::is_constructible_v<function<int(int, int)>, test_type>);
        static_assert(!std::is_constructible_v<function<int(int, int), (2 * pointer_size) - 1>, test_type>);
    }
    {
        struct test_type {
            int operator()(int arg1, int arg2) { return arg1 + arg2; }

            alignas(2 * pointer_size) void* val[4]{};
        };
        static_assert(sizeof(test_type) == 4 * pointer_size);
        static_assert(alignof(test_type) == 2 * pointer_size);

        static_assert(!std::is_constructible_v<function<int(int, int)>, test_type>);
        static_assert(!std::is_constructible_v<function<int(int, int), (4 * pointer_size) - 1>, test_type>);
        static_assert(std::is_constructible_v<function<int(int, int), 4 * pointer_size>, test_type>);
    }
}
