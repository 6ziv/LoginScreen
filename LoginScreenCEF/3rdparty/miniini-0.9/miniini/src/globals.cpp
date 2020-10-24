// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "typedefs.h"
#include "globals.h"

/// @cond PRIVATE
namespace miniini_private
{

#ifdef MINIINI_BENCH_EXTRA
#ifdef linux
///Used for high resolution file read time measurement.
miniini_private::ld bench_filetime = 0.0;
miniini_private::ld bench_alloctime = 0.0;
miniini_private::ld bench_loadtime = 0.0;
#endif
#endif

c comment = ';';                                                   

c namevalsep = '=';

c linesep[3] = {10, 13, 0};
}
/// @endcond

bool INISetComment(const char commentchar)
{
    switch(commentchar)
    {
        //Can't set to any character that is already used for something else.
        case ' ':
        case '\t':
        case 10:
        case 13:
        case '[':
        case ']':
        case '\0': 
            return false;
            break;
        default:
        {
            if(commentchar == miniini_private::namevalsep)
            {
                return false;
            }
            miniini_private::comment = commentchar;
            return true;
        }
        break;
    }
}

bool INISetSeparator(const char sep)
{
    switch(sep)
    {
        //Can't set to any character that is already used for something else.
        case ' ':
        case '\t':
        case 10:
        case 13:
        case '[':
        case ']':
        case '\0':
            return false;
            break;
        default:
        {
            if(sep == miniini_private::comment)
            {
                return false;
            }
            miniini_private::namevalsep = sep;
            return true;
        }
        break;
    }
}
