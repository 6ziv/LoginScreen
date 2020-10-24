// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include "typedefs.h"

/// @cond PRIVATE
namespace miniini_private
{
    #ifdef MINIINI_BENCH_EXTRA
    #ifdef linux
    #include <sys/time.h>
    inline ld GetTime()
    {
        timeval tv;
        gettimeofday(&tv, 0);
        return tv.tv_sec + (static_cast<ld>(tv.tv_usec) / 1000000.0);
    }
    #endif
    #endif
}
/// @endcond
#endif
