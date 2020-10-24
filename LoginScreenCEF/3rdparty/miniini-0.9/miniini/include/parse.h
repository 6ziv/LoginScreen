// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <cassert>
#include <cfloat>

#include "typedefs.h"               

#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

namespace miniini_private
{
/// @cond PRIVATE
//minimum and maximum 32bit int values, borrowed from C99 limits.h
#define INT32_MAX (2147483647)
#define INT32_MIN (-2147483647 - 1)

//repeat a piece of code 10 times
#define FOR_10(instr) \
{\
  instr;\
  instr;\
  instr;\
  instr;\
  instr;\
  instr;\
  instr;\
  instr;\
  instr;\
  instr;\
}

//decimal orders of magnitude using in int parsing
const s64 orders_of_magnitude_int [10] = {1, 10, 100, 1000, 10000, 100000, 
                                          1000000, 10000000, 100000000, 
                                          1000000000};

//represents status of conversion in parsing methods
enum ConversionStatus
{
    CONV_OK,
    CONV_WAR_OVERFLOW,
    CONV_WAR_REDUNANT,
    CONV_ERR_TYPE
};

/** Parses given string as a 32bit int
 * It is assumed that the string contains no spaces. Only decimal format is 
 * supported, i.e. no support for hexadecimal or octal. There is no support for
 * thousand separator characters. If the parsed integer is out of range of 32bit
 * integers, largest or smallest possible 32bit integer is written to out and
 * CONV_WAR_OVERFLOW is returned. In debug mode, if there were redunant 
 * characters in the string, CONV_WAR_REDUNANT is returned. If the string 
 * couldn't be parsed, CONV_ERR_TYPE is returned and out is not altered. 
 * Otherwise, CONV_OK is returned.
 * @param string String to parse
 * @param out Reference to int to write to
 * @return status of the conversion
 */
inline ConversionStatus ParseInt(const c * string, s32 & out)
{
    //process sign character, if any
    
    //1 if the number we're parsing is positive, -1 if negative
    i sign = 1;
    if(*string == '-')
    {
        sign = -1;
        ++string;
    }
    else if(*string == '+')
    {
        ++string;
    }

    //skip any leading zeroes
    
    //if there is at least one leading zero, this will be true.
    //If the number is composed of only leading zeroe, which are skipped, 
    //(e.g. number 0), this is used to check that the string contains a number
    //instead of containing only non-parsable characters.
    bool zero = false;
    //first zero
    if(*string == '0')
    {
        zero = true;
        ++string;
        //any more zeroes
        while (*string == '0')
        {
            ++string;
        }
    }

    //stores digits of the parsed number (as integers). 
    //32 bit integer can have at most 10 digits.
    static i digits [10];
    ui digitcount = 0;

    //read digits
    register c digitchar;
    FOR_10
    (
        digitchar = *string;
        if(digitchar < '0' || digitchar > '9')
        {
            goto READ_END;
        }
        digits[digitcount] = digitchar - '0';
        ++digitcount;
        ++string;
    )
    //if 11th char is still a number, we're overflowing
    if(*string >= '0' && *string <= '9' )
    {
        if(sign == 1)
        {
            out = INT32_MAX;
        }
        else
        {
            out = INT32_MIN;
        }
        return CONV_WAR_OVERFLOW;
    }

    READ_END:

    //if digitcount is 0, and there weren't any leading zeroes (which don't add to digitcount),
    //we have an error.
    if(digitcount == 0)
    {
        if(zero)
        {
            //The number only contains zeroes, so it is 0
            out = 0;
            return CONV_OK;
        }
        else
        {
            //out is not altered
            return CONV_ERR_TYPE;
        }
    }
    
    //using 64 bit so we can check for overflows easily
    s64 temp = 0;

    //parse the digits
    for(ui digit = 0; digit < digitcount; ++digit)
    {
        temp += static_cast<s64>(digits[digit] * orders_of_magnitude_int[digitcount - digit - 1]);
    }

    //apply the sign
    temp *= sign;
    //handle overflows
    if(temp > INT32_MAX)
    {
        out = INT32_MAX;
        return CONV_WAR_OVERFLOW;
    }
    else if(temp < INT32_MIN)
    {
        out = INT32_MIN;
        return CONV_WAR_OVERFLOW;
    }

    out = static_cast<s32>(temp);
    #ifdef MINIINI_DEBUG
    //if we're not yet at the trailing zero, this is a float or has redunant chars
    if(*string != '\0')
    {
        return CONV_WAR_REDUNANT;
    }
    #endif
    return CONV_OK;
}

/** Parses given string as the whole number part of a float
 * It is assumed that the string contains no spaces. Only decimal format is 
 * supported, i.e. no support for hexadecimal or octal. There is no support for
 * thousand separator characters. If the parsed float has more digits in the
 * whole number part than the largest possible 32bit float, CONV_WAR_OVERFLOW is
 * returned and out is not altered. In debug mode, if this sequence of 
 * characters doesn't end with a decimal point, CONV_WAR_REDUNANT is returned. 
 * Even an empty sequence ending by a decimal point (like in .5) is parsed 
 * correctly as 0. If the string couldn't be parsed, CONV_ERR_TYPE is returned.
 * The string pointer is passed through a reference so its alterations affect
 * the caller.
 * @param string Reference to string to parse
 * @param out Reference to float to write to
 * @return status of the conversion
 */
inline ConversionStatus ParseFloatBase(const c *& string, ld & out)
{
    //skip any leading zeroes
    
    //if there is at least one leading zero, this will be true.
    //If the whole part is composed of only leading zeroes, which are skipped, 
    //(e.g. in 0.0), this is used to check that the string contains a number
    //instead of containing only non-parsable characters.
    bool zero = false;
    //first zero
    if(*string == '0')
    {
        zero = true;
        ++string;
        //any more zeroes
        while (*string == '0')
        {
            ++string;
        }
    }
    
    //stores digits of the parsed number (as integers). 
    //32 bit float can have at most 39 digits before decimal point.
    static i digits [39];
    ui digitcount = 0;

    //read digits
    while(*string >= '0' && *string <= '9')
    {
        //if we already have 39 digits and there's another one, we're overflowing
        if(digitcount == 39)
        {
            return CONV_WAR_OVERFLOW;
        }
        digits[digitcount] = *string - '0';
        ++digitcount;
        ++string;
    }
    
    if(digitcount == 0)
    {
        //digitcount is 0 but there were leading zeroes so the whole number
        //part is 0
        if(zero)
        {
            //The number only contains zeroes, so it is 0
            out = 0.0;
            if(*string == '.' || *string == '\0')
            {
                return CONV_OK;
            }
            return CONV_WAR_REDUNANT;
        }
        //digitcount is 0, there weren't leading zeroes but we stopped at a 
        //decimal point instead of an invalid character (e.g. .5 instead of X5) 
        //so the whole number part is still a valid 0
        else if(*string == '.')
        {
            out = 0.0;
            return CONV_OK;
        }
        //we stopped at an invalid character without any digits or leading 
        //zeroes, so we have an error
        else 
        {
            //out is not altered
            return CONV_ERR_TYPE;
        }
    }

    ld temp = 0.0;

    //parse the digits
    ld order_of_magnitude = 1.0;
    for(i digit = digitcount - 1; digit >= 0; --digit)
    {
        temp += digits[digit] * order_of_magnitude;
        order_of_magnitude *= 10.0;
    }

    //overflow will be checked by the caller (we're using long doubles 
    //temporarily, which are precise enough as we're only parsing the first
    //39 digits)
    out = temp;

    #ifdef MINIINI_DEBUG
    if(*string != '.' && *string != '\0')
    {
        return CONV_WAR_REDUNANT;
    }
    #endif
    return CONV_OK;
}

/** Parses given string as the fraction part of a float
 * It is assumed that the string contains no spaces. Only decimal format is 
 * supported, i.e. no support for hexadecimal or octal. There is no support for
 * thousand separator characters. If the parsed float has more digits in the
 * fraction number part than the largest possible 32bit float, any further 
 * digits are ignored. In debug mode, if there are any redunant non-digit 
 * characters, only digits up to the first redunant character are read and
 * CONV_WAR_REDUNANT is returned. If there are no digits, 0.0 is written to out 
 * and CONV_OK is returned. If there were no problems, CONV_OK is returned.
 * The string pointer is passed through a reference so its alterations affect
 * the caller.
 * @param string Reference to string to parse
 * @param out Reference to float to write to
 * @return status of the conversion
 */
inline ConversionStatus ParseFloatFraction(const c *& string, ld & out)
{
    out = 0.0;
    //stores digits of the parsed number (as integers). 
    //32 bit float can have at most 38 digits after the decimal point.
    static i digits [38];
    ui digitcount = 0;
    //read digits
    while(*string >= '0' && *string <= '9')
    {
        if(digitcount == 38)
        {
            break;
        }
        digits[digitcount] = *string - '0';
        ++digitcount;
        ++string;
    }
    ld temp = 0.0;
    
    //even if there are zero digits, there is no error, fraction is 0.0

    //parse digits
    ld order_of_magnitude = 0.1;
    for(ui digit = 0; digit < digitcount; ++digit)
    {
        temp += digits[digit] * order_of_magnitude;
        order_of_magnitude *= 0.1;
    }

    out = temp;

    #ifdef MINIINI_DEBUG
    if(*string != '\0')
    {
        return CONV_WAR_REDUNANT;
    }
    #endif
    return CONV_OK;
}

/** Parses given string as a 32bit float
 * It is assumed that the string contains no spaces. Only decimal format is 
 * supported, i.e. no support for hexadecimal or octal. There is no support for
 * thousand separator characters. If the parsed float is out of range of 32bit
 * floats, largest or smallest possible float is written to out and 
 * CONV_WAR_OVERFLOW is returned. In debug mode, if there are redunant 
 * characters in the parsed string, it is parsed up to the first redunant 
 * character, result written to out and CONV_WAR_REDUNANT is returned.
 * (when not in debug mode, CONV_OK is returned). If the string could not be
 * parsed, CONV_ERR_TYPE is returned and out is not altered. Otherwise CONV_OK 
 * is returned and the result written to out.
 * @param string Reference to string to parse
 * @param out Reference to float to write to
 * @return status of the conversion
 */
inline ConversionStatus ParseFloat(const c * string, f & out)
{
    //1 if the number we're parsing is positive, -1 if negative
    ld sign = 1.0;
    //process sign character, if any
    if(*string == '-')
    {
        sign = -1.0;
        ++string;
    }
    else if(*string == '+')
    {
        ++string;
    }

    ld base, fraction; 
    //First parse the whole number part of the float.
    ConversionStatus statbase = ParseFloatBase(string, base); 
    if(statbase == CONV_ERR_TYPE)
    {
        return CONV_ERR_TYPE;
    }
    else if(statbase == CONV_WAR_OVERFLOW)
    {
        if(sign > 0.0)
        {
            out = FLT_MAX;
        }
        else
        {
            out = -FLT_MAX;
        }
        return CONV_WAR_OVERFLOW;
    }
    #ifdef MINIINI_DEBUG
    else if(statbase == CONV_WAR_REDUNANT)
    {
        out = static_cast<f>(sign * base);
        return CONV_WAR_REDUNANT;
    }
    #endif
    //there were no warnings but there wasn't a decimal point so we're done 
    //parsing
    if(*string != '.')
    {
        out = static_cast<f>(sign * base);
        return CONV_OK;
    }
    //skip the decimal point
    ++string;
    //First parse the fraction part of the float
    ConversionStatus statfraction = ParseFloatFraction(string, fraction); 
    //join the whole number and fraction parts
    ld temp = fraction + base;
    if(temp > FLT_MAX)
    {
        temp = FLT_MAX;
        out = static_cast<f>(sign * temp);
        return CONV_WAR_OVERFLOW;
    }
    out = static_cast<f>(sign * temp);
    #ifdef MINIINI_DEBUG
    if(statfraction == CONV_WAR_REDUNANT)
    {
        return CONV_WAR_REDUNANT;
    }
    #endif
    return CONV_OK;
}
#undef INT32_MAX
#undef INT32_MIN

#undef FOR_10
/// @endcond 
}
#endif                                                              
