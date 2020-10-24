// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include "typedefs.h"

/// @cond PRIVATE
namespace miniini_private
{

/** Reallocates given buffer to double capacity.
 * @param buf Buffer to reallocate.
 * @param cap Capacity of the buffer.
 * @param size Size of (number of elements in) the buffer.
 * @param type Data type stored in the buffer.
 */
#define MINIINI_REALLOCATE(buf, cap, size, type)\
{\
    cap *= 2;\
    type * tempbuf = new type[cap];\
    memcpy(tempbuf, buf, size * sizeof(type));\
    delete [] buf;\
    buf = tempbuf;\
}


/** Get to the first character of the next line in a character buffer.
 * @param currentchar Pointer to the current character in a buffer.
 * @return pointer to the first character of the next line, or a null character if found.
 */
inline const c * NextLine(const c * currentchar)
{
    register c ch;
    //searching for first newline
    for(; ; ++currentchar)
    {
        ch = *currentchar;
        if(ch == 13 || ch == 10)
        {
            break;
        }
        else if(ch == '\0')
        {
            return currentchar;
        }
    }
    //searching for first non-newline
    for(; ; ++currentchar)
    {
        ch = *currentchar;
        if(ch != 13 && ch != 10)
        {
            return currentchar;
        }
    }
}

/** Get a string to use for inserting or searching from a ptr to INISection.
 * @param ptr Pointer to INISection to get string from.
 * @return String corresponding to given INISction.
 */
static inline const c * GetName(const INISection * ptr)
{
    return ptr->GetName();
}

/** Get a string to use for inserting or searching from a char pointer.
 * @param ptr Pointer to c to get string from.
 * @return String corresponding to given c pointer.
 */
static inline const c * GetName(const c * ptr)
{
    return ptr;
}

/** Inserts an ptr to element to a sorted array so that the array stays sorted.
 * Elements are sorted by string values returned by GetName function.
 * @note that array must have space for at least length + 1 elements.
 * @param array Array of strings to insert to.
 * @param length Number of elements in array.
 * @param elem Element to insert.
 */
template <typename T>
inline bool Insert(T * * array, const ui length, register T * elem)
{
    //Binary search to find where to insert elem
    ui min = 0;
    ui max = length;
    ui mid;
    register c c1, c2;
    register const c * str1, * str2;
    while(min < max)
    {
        mid = (max + min) / 2;
        //string comparison
        str1 = GetName(elem);
        str2 = GetName(array[mid]);
        do
        {
            c1 = *str1++;
            c2 = *str2++;
        }
        while(c1 && c1 == c2);
        if(c1 > c2)
        {
            min = mid + 1;
        }
        else
        {
            max = mid;
        }
    }

    //when we're finished searching, min is the index where we can put the elem.
    ui movesize = length - min;
    //this is where we insert elem
    register T * * ins = array + min;
    //memmove is slow for small blocks of data
    if(movesize < 28)
    {
        register T * * out = array + length;
        register T * * in = out - 1;
        for(;in >= ins; --in, --out )
        {
            *out = *in;
        }
    }
    else
    {
        memmove(ins + 1, ins, movesize * sizeof(T *));
    }
    *ins = elem;
    return true;
}

#ifdef __GNUC__
template <typename T>
inline i BinarySearch(const T * const * const array, const ui length, const c * name)
       __attribute__((always_inline))
#endif
;
/** Find requested element in an array using binary search.
 * Elements are searched by string values returned by GetName function.
 * @param array Array to search in.
 * @param length Number of elements in array.
 * @param name Name of the element to search for.
 * @return index of requested element if found.
 * @return -1 if not found.
 */
template <typename T>
inline i BinarySearch(const T * const * const array, const ui length, const c * name)
{
    //Start, middle and end of currently searched interval
    i min, mid, max;
    min = 0;
    max = length - 1;
    //We copy characters used in string comparison here
    register c c1, c2;
    register const c * str1, * str2;
    while(min <= max)
    {
        mid = (max + min) / 2;
        str1 = name;
        str2 = GetName(array[mid]);
        do
        {
            c1 = *str1++;
            c2 = *str2++;
        }
        while(c1 && c1 == c2);
        //tag is greater than Tags[mid]
        if(c1 > c2)
        {
            min = mid + 1;
        }
        else if(c1 < c2)
        {
            max = mid - 1;
        }
        else
        {
            //found tag
            return mid;
        }
    }
    //tag not found
    return -1;
}

}
/// @endcond
#endif
