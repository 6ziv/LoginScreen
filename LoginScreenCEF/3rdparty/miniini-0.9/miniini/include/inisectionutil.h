// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef INISECTIONUTIL_H_INCLUDED
#define INISECTIONUTIL_H_INCLUDED

#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <cassert>

#include "typedefs.h"               
#include "globals.h"
#include "log.h"
#include "linetoken.h"
#include "inisection.h"
#include "util.h"
#include "parse.h"

using namespace miniini_private;

ui INISection::temptagscap = 0;
c * * INISection::temptags = NULL;
ui INISection::tagcap = 0;
c * INISection::tagbuf = NULL;

/// @cond PRIVATE
///Perform given code 4 times. Used for manual loop unrolling.
#define FOR_4(instr) \
{\
  instr;\
  instr;\
  instr;\
  instr;\
}
/// @endcond

inline LineToken INISection::TagName(const c * & currentcharref, ui & tagsize)
{
    //ptr to the current character
    const c * currentchar = currentcharref;
    //current character
    register c ch;
    for(;;)
    {
        //reallocate header buffer if not enough space to add new chars
        //need 4 chars for unrolled part of the loop, 3 for trailing zeroes
        //since tag and value are stored in the same buffer separated by a zero
        //and the last tag is followed by two zeroes
        if(tagcap < tagsize + 7)
        {
            MINIINI_REALLOCATE(tagbuf, tagcap, tagsize, c);
        }
        //unrolled part of the loop (process 4 chars)
        FOR_4
        (
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
                    return LT_NAME;
                    break;
                }
                //header start found.
                case '[':
                {
                    currentcharref = currentchar;
                    return LT_HEADER;
                    break;
                }
                case '\0':
                {
                    currentcharref = currentchar;
                    return LT_NAME;
                    break;
                }
                default:
                {       
                    //value found, if tag name is not empty, start reading val
                    if(ch == namevalsep)
                    {
                        if(tagsize)
                        {
                            //adding trailing zero to the tag name
                            tagbuf[tagsize] = 0;
                            ++tagsize;
                            currentcharref = currentchar + 1;
                            return LT_VAL;
                        }
                        else
                        {
                            MINIINI_WARNING("Empty tag name.");
                            currentcharref = NextLine(currentchar);
                            return LT_NAME;
                        }
                    } 
                    if(ch == comment)
                    {
                        currentcharref = NextLine(currentchar);
                        return LT_NAME;
                    }
                    //add new char to tag name.
                    tagbuf[tagsize] = ch;
                    ++tagsize;
                    break;
                }     
            }
            ++currentchar;
        }
        )
    }
}

#undef FOR_4

inline ui INISection::TagValue(const c * & currentcharref, ui tagsize)
{
    //ptr to the current character
    const c * currentchar = currentcharref;
    //current character
    register c ch;
    //reading characters of value
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
                return tagsize;
                break;
            }
            case '\0':
            {
                currentcharref = currentchar;
                return tagsize;
                break;
            }
            case ',':
            {
                //need 2 more chars for trailing 0s
                if(tagcap < tagsize + 3)
                {
                    MINIINI_REALLOCATE(tagbuf, tagcap, tagsize, c);
                }
                //add new char to value
                tagbuf[tagsize] = '\0';
                ++tagsize;
                break;
            }
            default:
            {
                if(ch == comment)
                {
                    currentcharref = NextLine(currentchar);
                    return tagsize;
                }
                //need 2 more chars for trailing 0s
                if(tagcap < tagsize + 3)
                {
                    MINIINI_REALLOCATE(tagbuf, tagcap, tagsize, c);
                }
                //add new char to value
                tagbuf[tagsize] = ch;
                ++tagsize;
            }
        }
    }
}

inline bool INISection::Header(const c * & currentcharref)
{
    //ptr to the current character
    const c * currentchar = currentcharref;
    //current character
    register c ch;
    //Name doesn't skip [ so we skip it here
    ++currentchar;
    //searching for first newline
    for(; ; ++currentchar)
    {
        ch = *currentchar;
        switch(ch)
        {
            //CR, LF
            case 10:
            case 13:
            {
                //not a header
                currentcharref = NextLine(currentchar);
                return false;
                break;
            }
            case ']':
            {
                //if empty, ignore
                return static_cast<bool>(currentchar - currentcharref);
                break;
            }
            case '\0':
            {
                //not a header
                currentcharref = currentchar;
                return false;
                break;
            }
            default:
            {
                if(ch == comment)
                {
                    currentcharref = NextLine(currentchar);
                    return false;
                }
                break;
            }
        }
    }
}

inline ui INISection::ParseInts(const c * * strings, int * out, const ui numstrings)
{
    MINIINI_ASSERT(strings, "NULL pointer was passed as strings buffer to"
                            "INISection::ParseInts()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ParseInts()");
    //Ptr to current string in strings
    const c * const * str = strings;
    //When string reaches this, we've iterated over all the strings
    const c * const * maxstr = strings + numstrings;
    //Number of actual valid ints written to out
    unsigned elems = 0;
    //Currently parsed int
    s32 tempelem;
    //Status of current conversion
    ConversionStatus status;
    //Iterating through strings read by ReadStrings and converting them to ints
    for(; str < maxstr; ++str)
    {    
        status = ParseInt(*str, tempelem);
        if(status == CONV_ERR_TYPE)
        {
            MINIINI_ERROR("Non-integer value in an array or multi value tag " 
                          "where integer is expected. Terminating array / multi "
                          "value reading. Value: %s", *str);
            break;
        }
        #ifdef MINIINI_DEBUG
        else if(status == CONV_WAR_OVERFLOW)
        {
            MINIINI_WARNING("Integer value in an array or multi value tag out of "
                            "range."
                            "Value: %s", *str);
        }
        else if(status == CONV_WAR_REDUNANT)
        {
            MINIINI_WARNING("Redunant characters in a tag where integer is "
                            "expected. Reading integer. Value: %s", *str);
        }
        #endif
        out[elems] = static_cast<int>(tempelem);   
        ++elems;
    }
    return elems;
}

inline ui INISection::ParseFloats(const c * * strings, float * out, const ui numstrings)
{
    MINIINI_ASSERT(strings, "NULL pointer was passed as strings buffer to"
                            "INISection::ParseFloats()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ParseFloats()");
    //Ptr to current string in strings
    const c * const * string = strings;
    //When string reaches this, we've iterated over all the strings
    const c * const * const maxstring = strings + numstrings;
    //Number of actual valid floats written to out
    unsigned elems = 0;
    //Currently parsed float
    f tempelem;
    //Status of current conversion
    ConversionStatus status;
    //Iterating through strings read by ReadStrings and converting them to floats
    for(; string < maxstring; ++string)
    {    
        status = ParseFloat(*string, tempelem);
        if(status == CONV_ERR_TYPE)
        {
            MINIINI_ERROR("Non-float value in an array or multi value tag where "
                          "float is expected. Terminating array reading. "
                          "Value: %s", *string);
            break;
        }
        #ifdef MINIINI_DEBUG
        else if(status == CONV_WAR_OVERFLOW)
        {
            MINIINI_WARNING("Float value in an array or multi value tag out of " 
                            "range. Value: %s", *string);
        }
        else if(status == CONV_WAR_REDUNANT)
        {
            MINIINI_WARNING("Redunant characters in a tag where float is "
                            "expected. Reading float. Value: %s", *string);
        }
        #endif
        out[elems] = tempelem;   
        ++elems;
        
    }
    return elems;
}

inline ui INISection::ParseBools(const c * * strings, bool * out, const ui numstrings)
{
    MINIINI_ASSERT(strings, "NULL pointer was passed as strings buffer to"
                            "INISection::ParseBools()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ParseBools()");
    //Ptr to current string in strings
    const c * const * string = strings;
    //When string reaches this, we've iterated over all the strings
    const c * const * const maxstring = strings + numstrings;
    //Number of actual valid bools written to out
    unsigned elems = 0;
    //Iterating through strings read by ReadStrings and converting them to bools
    for(; string < maxstring; ++string, ++elems)
    {
        switch(*string[0])
        {
            case 't':
            case 'T':
            case 'y':
            case 'Y':
            case '1':
                out[elems] = true;
                break;
            case 'f':
            case 'F':
            case 'n':
            case 'N':
            case '0':
                out[elems] = false;
                break;
            default:
                MINIINI_ERROR("Non-bool value in a tag where bool is expected."
                              "Value: %s", *string);
                goto BREAK_FOR;
        }
    }
    BREAK_FOR:;
    return elems;
}
#endif
