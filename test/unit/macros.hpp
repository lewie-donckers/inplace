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

// ASSERT_THROW_OR_ABORT(statement, exception_type) will map to ASSERT_THROW when exceptions are enabled, or to
// ASSERT_DEATH when exceptions are disabled.
#ifdef INPLACE_EXCEPTIONS
#define ASSERT_THROW_OR_ABORT(statement, exception_type) ASSERT_THROW(statement, exception_type)
#else
#define ASSERT_THROW_OR_ABORT(statement, exception_type) ASSERT_DEATH(statement, ".*")
#endif
