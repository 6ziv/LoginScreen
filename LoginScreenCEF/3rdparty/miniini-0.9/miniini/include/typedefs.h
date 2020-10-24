// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef TYPEDEFS_H_INCLUDED
#define TYPEDEFS_H_INCLUDED

#include <stdint.h>  

namespace miniini_private
{

/// @cond PRIVATE
//int typedefs
//Typedefs for fastest ints
typedef uint_fast8_t              ub;
typedef int_fast8_t                b;
typedef uint_fast16_t             us;
typedef int_fast16_t               s;
typedef uint_fast32_t             ui;
typedef int_fast32_t               i;
typedef int_fast64_t              ll;
typedef uint_fast32_t            ull;

//Typedefs for chars
typedef char                       c;
typedef wchar_t                   wc;

//Typedefs for exact size ints
typedef int8_t                    s8;
typedef uint8_t                   u8;
typedef int16_t                  s16;
typedef uint16_t                 u16;
typedef int32_t                  s32;
typedef uint32_t                 u32;
typedef int64_t                  s64;
typedef uint64_t                 u64;

//Typedefs for biggest ints
typedef intmax_t                sbig;
typedef uintmax_t               ubig;

//Typedefs for floats
typedef float                      f;
typedef double                     d;
typedef long double               ld;
/// @endcond

}
#endif // TYPEDEFS_H_INCLUDED
