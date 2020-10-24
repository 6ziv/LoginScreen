// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <cstdio>
#include "typedefs.h"
#include "globals.h"

namespace miniini_private
{

/// @cond PRIVATE

//Ptr to the logging function
extern void(* __Log)(const char * const);

#ifdef MINIINI_DEBUG

#define MINIINI_LOGBUFSIZE 1024

extern c log_buf [MINIINI_LOGBUFSIZE];

#define MINIINI_LOG_BASE(...)\
    snprintf(miniini_private::log_buf, MINIINI_LOGBUFSIZE, __VA_ARGS__);\
    miniini_private::__Log(miniini_private::log_buf);

#define MINIINI_WARNING(...) \
{\
    if(miniini_private::__Log)\
    {\
        miniini_private::__Log("MiniINI WARNING:");\
        MINIINI_LOG_BASE(__VA_ARGS__);\
    }\
}

#define MINIINI_ERROR(...) \
{\
    if(miniini_private::__Log)\
    {\
        miniini_private::__Log("MiniINI ERROR:");\
        MINIINI_LOG_BASE(__VA_ARGS__);\
    }\
}

#define MINIINI_LOG(...) \
{\
    if(__Log)\
    {\
        miniini_private::__Log("MiniINI:");\
        MINIINI_LOG_BASE(__VA_ARGS__);\
    }\
}


#else

#define MINIINI_WARNING(...) ((void)0);

#define MINIINI_ERROR(...) ((void)0);

#define MINIINI_LOG(...) ((void)0);

#endif

/// @endcond 

}

/** Sets the logging function.
 * @param callback Pointer to the logging function - must return void and take a const char* parameter.
 */
void INILogCallback(void(* callback)(const char * const));

#endif
