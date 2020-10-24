// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED

#include <cstdlib>
#include <cassert>

#include "log.h"

namespace miniini_private
{
/// @cond PRIVATE
///Custom assert macro that will emit a MiniINI error with specified messaga and then trigger standard assert.
#ifndef NDEBUG
#define MINIINI_ASSERT(expr, message)\
    do\
    {\
        if(!(expr))\
        {\
            MINIINI_ERROR("Assertion failed: '%s' in file %s at line %d "\
                          "\nMessage: %s", #expr, __FILE__, __LINE__, message);\
            assert(expr);\
        }\
    }\
    while(false);
#else
#define MINIINI_ASSERT(expr, message) ((void)0);
#endif
/// @endcond 
}
#endif
