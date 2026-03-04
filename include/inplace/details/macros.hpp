// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

// TODO switch to include guards instead?
#pragma once

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#ifdef __GNUC__
#ifndef __clang__
#define INPLACE_COMPILER_GCC true
#endif
#endif

// TODO these are compiler specific. also support msvc?
#ifdef __GXX_RTTI
#define INPLACE_RTTI true
#endif

#ifdef __EXCEPTIONS
#define INPLACE_EXCEPTIONS true
#endif

#ifdef INPLACE_EXCEPTIONS
#define INPLACE_THROW_OR_ABORT(expr) throw expr
#else
#include <cstdlib>
#define INPLACE_THROW_OR_ABORT(expr) std::abort()
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)