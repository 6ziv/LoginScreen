// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef INISECTIONUTIL_H_INCLUDED
#define INISECTIONUTIL_H_INCLUDED

#include <cstdio>
#include <cstring>

#include "typedefs.h"
#include "inisection.h"
#include "inifile.h"
#include "util.h"

using namespace miniini_private;

/** Tries to read a header from line starting by given character pointer.
 * Updates character pointer to point to next line.
 * If needed, reallocates header buffer and updates its pointer and capacity.
 * @param currentcharref Reference to pointer to current character in a buffer.
 * @param header Reference to buffer where to read the header name (if any) to.
 * @param headercap Reference to the capacity of header buffer.
 * @return size of header or 0 if there is no header in the line or its size is 0 (which is invalid)
 */
inline ui INIFile::Header(const c * & currentcharref, c * & header, ui & headercap)
{
    //ptr to the current character
    const c * currentchar = currentcharref;
    //current character
    register c ch;
    //size of header in characters
    ui headersize = 0;
    //searching for [
    for(; ; ++currentchar)
    {
        ch = *currentchar;
        switch(ch)
        {
            case ' ':
            case '\t':
            {
                //ignore spaces
                break;
            }
            //CR, LF
            case 10:
            case 13:
            {
                currentcharref = NextLine(currentchar);
                return 0;
                break;
            }
            //header start found, we can start reading header
            case '[':
            {
                //move to next char
                ++currentchar;
                goto HEADER_START;
            }
            case '\0':
            {
                currentcharref = currentchar;
                return 0;
                break;
            }
            default:
            {
                //comment char
                if(ch == comment)
                {
                    currentcharref = NextLine(currentchar);
                    return 0;
                }
                break;
            }
        }
    }
    HEADER_START:;
    //searching for ] and reading header characters
    for(; ; ++currentchar)
    {
        ch = *currentchar;
        switch(ch)
        {
            case ' ':
            case '\t':
            {
                //ignore spaces
                break;
            }
            //CR, LF
            case 10:
            case 13:
            {
                currentcharref = NextLine(currentchar);
                return 0;
                break;
            }
            //header end found, we're finished reading
            case ']':
            {
                header[headersize] = 0;
                currentcharref = NextLine(currentchar);
                return headersize;
                break;
            }
            case '\0':
            {
                currentcharref = currentchar;
                return 0;
                break;
            }
            default:
            {
                if(ch == comment)
                {
                    currentcharref = NextLine(currentchar);
                    return 0;
                }
                //reallocate header buffer if not enough space to add new char
                //need 1 more char for trailing 0
                if(headercap < headersize + 2)
                {
                    MINIINI_REALLOCATE(header, headercap, headersize, c);
                }
                //add new char to header
                header[headersize] = ch;
                ++headersize;
                break;
            }
        }
    }
}
#endif
