// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <cstring>
#include <cerrno>
#include <cstdlib>

#include "typedefs.h"               
#include "globals.h"
#include "log.h"
#include "inisection.h"
#include "util.h"
#include "inisectionutil.h"

using namespace miniini_private;

void INISection::Init(const c * const sectionname, const c * * const currentcharptr, 
                      Allocator * const alloc)
{
    MINIINI_ASSERT(sectionname, "NULL pointer was passed as section name to"
                                "INISection::Init()");
    MINIINI_ASSERT(alloc, "NULL pointer was passed as allocator to"
                          "INISection::Init()");
    MINIINI_ASSERT(currentcharptr, "NULL pointer was passed as current char "
                                   "pointer to INISection::Init()");
    //If *currentcharptr is 0 (null character), we're at the end of buffer
    MINIINI_ASSERT(*currentcharptr, "An empty buffer was passed to "
                                    "INISection::Init()");
    MINIINI_ASSERT(temptags, "INISection::Init() : temptags is not allocated");
    MINIINI_ASSERT(tagbuf, "INISection::Init() : tagbuf is not allocated");

    //Copying allocator ptr
    Alloc = alloc;
    //Copying name of the section
    const ui namelen = strlen(sectionname) + 1;
    MINIINI_ASSERT(namelen > 1, "Section name passed to INISection::Init() is empty");
    Name = Alloc->NewSpace(namelen);
    memcpy(Name, sectionname, namelen);
    //ptr to the current character in buffer
    const c * currentchar = *currentcharptr;
    //Iterating through lines in the buffer
    while(*currentchar != '\0')
    {
        //size of tag name (if any) read
        ui namesize = 0;
        //goes to start of next line if nothing found, 
        //if value found, stops right after name=value separator,
        //if header found, stays at beginning of line
        LineToken token = TagName(currentchar, namesize);
        //value found (this line is a name=value pair)
        if(token == LT_VAL)
        {
            //size of name and value
            ui tagsize = TagValue(currentchar, namesize);
            //value is empty
            if(namesize == tagsize)
            {
                MINIINI_WARNING("Empty value in a tag (no characters after "
                                "name=value separator. Ignoring. Section: %s", Name);
                continue;
            }
            //adding trailing zero to the tag value
            tagbuf[tagsize] = tagbuf[tagsize + 1] = 0;
            tagsize += 2;
            //if needed, reallocate the temp tags buffer
            if(Length >= temptagscap)
            {
                MINIINI_REALLOCATE(temptags, temptagscap, Length, c *);
            }
            //Add new tag to temp tags buffer. 
            c * newtag = Alloc->NewSpace(tagsize);
            memcpy(newtag, tagbuf, tagsize * sizeof(c));
            if(Insert(temptags, Length, newtag))
            {
                ++Length;
            }
            else
            {
                Alloc->DeleteSpace(newtag);
            }
        }
        //header found
        else if(token == LT_HEADER)
        {
            //if this line is a header, we're finished loading the section
            if(Header(currentchar))
            {
                break;
            }
            //else Header() leaves currentchar at the start of next line
        }
    }
    //Updating line token ptr of the caller
    *currentcharptr = currentchar;
    #ifdef MINIINI_DEBUG
    //There are no tags in the section
    if(!Length)
    {
        //only print empty section warning if this is not the [DEFAULT] section
        if(strcmp(Name, "[DEFAULT]"))
        {
            MINIINI_WARNING("Empty section. Section: %s", Name);
        }
    }
    #endif
    //Copy temp lines buffer to Lines
    Tags = new c * [Length];
    memcpy(Tags, temptags, Length * sizeof(c *));
}

INISection::~INISection()
{
    //There is no need to tell allocator to delete data here,
    //since this is only called on INIFile destruction and that's
    //when Allocator gets destroyed and deletes the data anyway.
    //However, if/when section deletion is implemented, there
    //will need to be a separate dtor to handle that case.
    if(Tags)
    {
        delete [] Tags;
    }
}

inline bool INISection::ReadString(const char * name, const char * & out) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as tag name to"
                         "INISection::ReadString()");
    i pos;
    //if an empty string ("") is passed, we're using the tag that the 
    //iteration index points to
    if(*name == '\0')
    {
        MINIINI_ASSERT(Iter >= static_cast<i>(0) && Iter < static_cast<i>(Length), 
                       "Called INISection::ReadString with iteration index "
                       "out of range");
        pos = Iter;
    }
    else
    {
        pos = BinarySearch(Tags, Length, name);
    }
    if(pos >= 0)
    {
        out = Tags [pos] + strlen(Tags[pos]) + 1;
        return true;
    }
    return false;
}

bool INISection::ReadInt(const char * const name, int & out) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as tag name to"
                         "INISection::ReadInt()");
    const c * valstr;
    //Requested tag does not exist
    if(!ReadString(name, valstr))
    {
        return false;
    }
    s32 tempout;
    ConversionStatus status = ParseInt(valstr, tempout);
    if(status == CONV_ERR_TYPE)
    {
        MINIINI_ERROR("Non-integer value in a tag where integer is expected."
                      "Section: %s Tag (if known): %s Value: %s", 
                      Name, name, valstr);
        return false;
    }
    #ifdef MINIINI_DEBUG
    else if(status == CONV_WAR_OVERFLOW)
    {
        MINIINI_WARNING("Integer value out of range."
                        "Section: %s Tag (if known): %s Value: %s", 
                        Name, name, valstr);
    }
    else if(status == CONV_WAR_REDUNANT)
    {
        MINIINI_WARNING("Redunant characters in a tag where integer is "
                        "expected. Reading integer. "
                        "Section: %s Tag (if known): %s Value: %s", 
                        Name, name, valstr);
    }
    #endif
    out = static_cast<int>(tempout);
    return true;
}

bool INISection::ReadFloat(const char * const name, float & out) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as tag name to"
                         "INISection::ReadFloat()");
    const c * valstr;
    //Requested tag does not exist
    if(!ReadString(name, valstr))
    {
        return false;
    }

    f tempout;
    ConversionStatus status = ParseFloat(valstr, tempout);
    if(status == CONV_ERR_TYPE)
    {
        MINIINI_ERROR("Non-float value in a tag where float is expected."
                      "Section: %s Tag (if known): %s Value: %s", 
                      Name, name, valstr);
        return false;
    }
    #ifdef MINIINI_DEBUG
    else if(status == CONV_WAR_OVERFLOW)
    {
        MINIINI_WARNING("Float value out of range."
                        "Section: %s Tag (if known): %s Value: %s", 
                        Name, name, valstr);
    }
    else if(status == CONV_WAR_REDUNANT)
    {
        MINIINI_WARNING("Redunant characters in a tag where float is "
                        "expected. Reading float."
                        "Section: %s Tag (if known): %s Value: %s",
                        Name, name, valstr);
    }
    #endif
    out = static_cast<float>(tempout);
    return true;
}

bool INISection::ReadBool(const char * const name, bool & out) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as tag name to"
                         "INISection::ReadBool()");
    const c * valstr;
    //Requested tag does not exist
    if(!ReadString(name, valstr))
    {
        return false;
    }
    //Parsing bool using the first character only
    switch(valstr[0])
    {
        case 't':
        case 'T':
        case 'y':
        case 'Y':
        case '1':
            out = true;
            return true;
        case 'f':
        case 'F':
        case 'n':
        case 'N':
        case '0':
            out = false;
            return true;
        default:
            MINIINI_ERROR("Non-bool value in a tag where bool is expected."
                          "Section: %s Tag (if known): %s Value: %s", 
                          Name, name, valstr);
            return false;
    }
    return false;
}

unsigned INISection::MultiValSize(const char * const name) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as tag name to"
                         "INISection::MultiValSize()");
    //ptr to current position in tag
    const c * tag;
    //Number of elements in the tag
    ui elems = 0;  
    i pos;
    //if an empty string ("") is passed, we're using the tag that the 
    //iteration index points to
    if(*name == '\0')
    {
        MINIINI_ASSERT(Iter >= static_cast<i>(0) && Iter < static_cast<i>(Length), 
                       "Called INISection::MultiValSize() with iteration index "
                       "out of range");
        pos = Iter;
    }
    else
    {
        pos = BinarySearch(Tags, Length, name);
    }
    if(pos >= 0)
    {
        //get to the start of the first value
        tag = Tags[pos];
        while(*tag != '\0')
        {
            ++tag;
        }
        ++tag;
        //now we're at the first char of value
        while(*tag != '\0')
        {
            ++elems;
            while(*tag != '\0')
            {
                ++tag;
            }
            ++tag;
            //now we're at the char after the trailing zero of previous value
        }
        return static_cast<unsigned>(elems);
    }
    return 0;
}

unsigned INISection::ReadMultiString(const char * const name, const char * * out, 
                                     const unsigned cap) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as tag name to"
                         "INISection::ReadMultiString()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ReadMultiString()");
    //ptr to current position in tag
    const c * tag;
    //Number of elements in the tag
    ui elems = 0;  
    i pos;
    //if an empty string ("") is passed, we're using the tag that the 
    //iteration index points to
    if(*name == '\0')
    {
        MINIINI_ASSERT(Iter >= static_cast<i>(0) && Iter < static_cast<i>(Length), 
                       "Called INISection::ReadMultiString() with iteration index "
                       "out of range");
        pos = Iter;
    }
    else
    {
        pos = BinarySearch(Tags, Length, name);
    }
    //tag exists
    if(pos >= 0)
    {
        //get to the start of the first value
        tag = Tags[pos];
        while(*tag != '\0')
        {
            ++tag;
        }
        ++tag;
        //now we're at the first char of value
        while(*tag != '\0')
        {
            //out of capacity, don't read any more values
            if(elems == cap)
            {
                MINIINI_WARNING("Multi value tag element out of capacity."
                                "Section: %s Tag (if known): %s Capacity: %d", 
                                Name, name, cap);
                break;
            }
            //add element
            out[elems] = tag;
            ++elems;
            //get to the zero after the current value
            while(*tag != '\0')
            {
                ++tag;
            }
            //go to the next character. if it's 0, we're at the end of the tag, 
            //otherwise, there is another value.
            ++tag;
            //now we're at the char after the trailing zero of previous value
        }
        return static_cast<unsigned>(elems);
    }
    return 0;

}

unsigned INISection::ReadMultiInt(const char * const name, int * out, 
                                  const unsigned cap) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as tag name to"
                         "INISection::ReadMultiInt()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ReadMultiInt()");
    //Array of value strings to be converted to ints
    const c * * const valstrs = new const c * [cap];
    //Number of strings read by ReadStrings to valstrs
    const ui tempelems = ReadMultiString(name, valstrs, cap);
    ui output = ParseInts(valstrs, out, tempelems);
    delete [] valstrs;
    return static_cast<unsigned>(output);
}

unsigned INISection::ReadMultiFloat(const char * const name, float * out, 
                                    const unsigned cap) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as tag name to"
                         "INISection::ReadMultiFloat()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ReadMultiFloat()");
    //Array of value strings to be converted to floats
    const c * * const valstrs = new const c * [cap];
    //Number of strings read by ReadStrings to valstrs
    const ui tempelems = ReadMultiString(name, valstrs, cap);
    ui output = ParseFloats(valstrs, out, tempelems);
    delete [] valstrs;
    return static_cast<unsigned>(output);
}

unsigned INISection::ReadMultiBool(const char * const name, bool * out, 
                                   const unsigned cap) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as tag name to"
                         "INISection::ReadMultiBool()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ReadMultiBool()");
    //Array of value strings to be converted to floats
    const c * * const valstrs = new const c * [cap];
    //Number of strings read by ReadStrings to valstrs
    const ui tempelems = ReadMultiString(name, valstrs, cap);
    ui output = ParseBools(valstrs, out, tempelems);
    delete [] valstrs;
    return static_cast<unsigned>(output);
}

#ifndef MINIINI_NO_STL
unsigned INISection::ReadMultiString(const std::string & name, 
                                     std::vector<std::string> & out) const
{
    const char * cname = name.c_str();
    unsigned numelems = MultiValSize(cname);
    //Read data to this array first
    const char * * tempstrs = new const char * [numelems];
    numelems = ReadMultiString(cname, tempstrs, numelems);
    out.reserve(numelems);
    //Move data to output vector
    for(ui str = 0; str < numelems; ++str)
    {
        out.push_back(std::string(tempstrs[str]));
    }
    delete [] tempstrs;
    return numelems;
}

unsigned INISection::ReadMultiInt(const std::string & name, 
                                  std::vector<int> & out) const
{
    const char * cname = name.c_str();
    unsigned numelems = MultiValSize(cname);
    //Read data to this array first
    int * tempints = new int [numelems];
    numelems = ReadMultiInt(cname, tempints, numelems);
    out.reserve(numelems);
    //Move data to output vector
    for(ui in = 0; in < numelems; ++in)
    {
        out.push_back(tempints[in]);
    }
    delete [] tempints;
    return numelems;
}

unsigned INISection::ReadMultiFloat(const std::string & name, 
                                   std::vector<float> & out) const
{
    const char * cname = name.c_str();
    unsigned numelems = MultiValSize(cname);
    //Read data to this array first
    float * tempfloats = new float [numelems];
    numelems = ReadMultiFloat(cname, tempfloats, numelems);
    out.reserve(numelems);
    //Move data to output vector
    for(ui fl = 0; fl < numelems; ++fl)
    {
        out.push_back(tempfloats[fl]);
    }
    delete [] tempfloats;
    return numelems;
}

unsigned INISection::ReadMultiBool(const std::string & name, 
                                   std::vector<bool> & out) const
{
    const char * cname = name.c_str();
    unsigned numelems = MultiValSize(cname);
    //Read data to this array first
    bool * tempbools = new bool [numelems];
    numelems = ReadMultiBool(cname, tempbools, numelems);
    out.reserve(numelems);
    //Move data to output vector
    for(ui bo = 0; bo < numelems; ++bo)
    {
        out.push_back(tempbools[bo]);
    }
    delete [] tempbools;
    return numelems;
}
#endif

unsigned INISection::ArraySize(const char * const name) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as base tag name to"
                         "INISection::ArraySize()");
    ui namelen = strlen(name);
    //ptr to current tag
    const c * tag;
    //Index of current array element
    s32 elemidx;  
    ui indicescap = 32;
    ui indicessize = 0;
    //Stores indices of found elements
    ui * indices = new ui [indicescap];
    //Status of conversion of int in current tag name to index of array elemenent
    ConversionStatus status;
    for(ui idx = 0; idx < Length; ++idx)
    {
        tag = Tags[idx];
        //if tag name starts by name
        if(!strncmp(name, tag, namelen))
        {
            status = ParseInt(tag + namelen, elemidx);
            if(status == CONV_OK)
            {
                //Reallocating indices if out of space
                if(indicessize >= indicescap)
                {
                    MINIINI_REALLOCATE(indices, indicescap, indicessize, ui);
                }
                //Correct index, counting this array element
                indices[indicessize] = elemidx - 1;
                ++indicessize;
            }
            #ifdef MINIINI_DEBUG
            //Huge (out of range) integer in tag name- 
            //Probably a different tag name or error
            else if(status == CONV_WAR_OVERFLOW)
            {
                MINIINI_WARNING("Integer in tag name out of range. Ignoring. "
                                "Section: %s Tag name: %s", Name, tag);
                continue;
            }
            //Redunant chars after int - probably a different tag name or error 
            else if(status == CONV_WAR_REDUNANT)
            {
                MINIINI_WARNING("Redunant characters after integer in tag name. "
                                "Ignoring. Section: %s Tag name: %s", Name, tag);
                continue;
            }
            #endif
        }
    }
    ui elems = 0;
    bool finished = false;
    //Count number of consecutive indices
    while(!finished)
    {
        finished = true;
        for(ui idx = 0; idx < indicessize; ++idx)
        {
            if(indices[idx] == elems)
            {
                ++elems;
                finished = false;
            }
        }
    }
    delete [] indices;
    return static_cast<unsigned>(elems);
}

unsigned INISection::ReadStrings(const char * const name, const char * * out, 
                                 const unsigned cap) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as base tag name to"
                         "INISection::ReadStrings()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ReadStrings()");
    ui namelen = strlen(name);
    //we fill out with zeroes, so we can search for null ptr to determine how 
    //many elements we've read
    memset(out, 0, sizeof(const char *) * cap);
    //ptr to current tag
    const c * tag;
    //Index of current array element
    s32 elemidx;  
    //Status of conversion of int in current tag name to index of array elemenent
    ConversionStatus status;
    for(ui idx = 0; idx < Length; ++idx)
    {
        tag = Tags[idx];
        //if tag name starts by name
        if(!strncmp(name, tag, namelen))
        {
            status = ParseInt(tag + namelen, elemidx);
            if(status == CONV_OK)
            {
                //Check for overflow
                if(elemidx > 0 && elemidx <= static_cast<s32>(cap))
                {
                    out[elemidx - 1] = (tag) + strlen(tag) + 1;
                }
                else
                {
                    MINIINI_WARNING("Array element out of capacity."
                                    "Section: %s Tag name: %s Capacity: %d", 
                                    Name, tag, cap);
                    continue;
                }
            }
            #ifdef MINIINI_DEBUG
            //Huge (out of range) integer in tag name- 
            //Probably a different tag name or error
            else if(status == CONV_WAR_OVERFLOW)
            {
                MINIINI_WARNING("Integer in tag name out of range. Ignoring. "
                                "Section: %s Tag name: %s", Name, tag);
                continue;
            }
            //Redunant chars after int - probably a different tag name or error 
            else if(status == CONV_WAR_REDUNANT)
            {
                MINIINI_WARNING("Redunant characters after integer in tag name. "
                                "Ignoring. Section: %s Tag name: %s", Name, tag);
                continue;
            }
            #endif
        }
    }
    //return number of valid elements read 
    //(if, say, elem 19 is missing, only elems 0-18 are valid)
    for(unsigned elem = 0; elem < cap; ++elem)
    {
        if(!out[elem])
        {
            return elem;
        }
    }
    return cap;
}

unsigned INISection::ReadInts(const char * const name, int * out, 
                              const unsigned cap) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as base tag name to"
                         "INISection::ReadInts()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ReadInts()");
    //Array of value strings to be converted to ints
    const c * * const valstrs = new const c * [cap];
    //Number of strings read by ReadStrings to valstrs
    const ui tempelems = ReadStrings(name, valstrs, cap);
    ui output = ParseInts(valstrs, out, tempelems);
    delete [] valstrs;
    return static_cast<unsigned>(output);
}       

unsigned INISection::ReadFloats(const char * const name, float * out, 
                                const unsigned cap) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as base tag name to"
                         "INISection::ReadFloats()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ReadFloats()");
    //Array of value strings to be converted to floats
    const c * * const valstrs = new const c * [cap];
    //Number of strings read by ReadStrings to valstrs
    const ui tempelems = ReadStrings(name, valstrs, cap);
    ui output = ParseFloats(valstrs, out, tempelems);
    delete [] valstrs;
    return static_cast<unsigned>(output);
}     

unsigned INISection::ReadBools(const char * const name, bool * out, 
                               const unsigned cap) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as base tag name to"
                         "INISection::ReadBools()");
    MINIINI_ASSERT(out, "NULL pointer was passed as output buffer to"
                        "INISection::ReadBools()");
    //Array of value strings to be converted to bools 
    const c * * const valstrs = new const c * [cap];
    //Number of strings read by ReadStrings to valstrs
    const ui tempelems = ReadStrings(name, valstrs, cap);
    ui output = ParseBools(valstrs, out, tempelems);
    delete [] valstrs;
    return static_cast<unsigned>(output);
}

#ifndef MINIINI_NO_STL
unsigned INISection::ReadStrings(const std::string & name, 
                                 std::vector<std::string> & out) const
{
    const char * cname = name.c_str();
    unsigned numelems = ArraySize(cname);
    //Read data to this array first
    const char * * tempstrs = new const char * [numelems];
    numelems = ReadStrings(cname, tempstrs, numelems);
    out.reserve(numelems);
    //Move data to output vector
    for(ui str = 0; str < numelems; ++str)
    {
        out.push_back(std::string(tempstrs[str]));
    }
    delete [] tempstrs;
    return numelems;
}

unsigned INISection::ReadInts(const std::string & name, 
                                 std::vector<int> & out) const
{
    const char * cname = name.c_str();
    unsigned  numelems = ArraySize(cname);
    //Read data to this array first
    int * tempints = new int [numelems];
    numelems = ReadInts(cname, tempints, numelems);
    out.reserve(numelems);
    //Move data to output vector
    for(ui in = 0; in < numelems; ++in)
    {
        out.push_back(tempints[in]);
    }
    delete [] tempints;
    return numelems;
}

unsigned INISection::ReadFloats(const std::string & name, 
                                 std::vector<float> & out) const
{
    const char * cname = name.c_str();
    unsigned  numelems = ArraySize(cname);
    //Read data to this array first
    float * tempfloats = new float [numelems];
    numelems = ReadFloats(cname, tempfloats, numelems);
    out.reserve(numelems);
    //Move data to output vector
    for(ui fl = 0; fl < numelems; ++fl)
    {
        out.push_back(tempfloats[fl]);
    }
    delete [] tempfloats;
    return numelems;
}

unsigned INISection::ReadBools(const std::string & name, 
                                 std::vector<bool> & out) const
{
    const char * cname = name.c_str();
    unsigned numelems = ArraySize(cname);
    //Read data to this array first
    bool * tempbools = new bool [numelems];
    numelems = ReadBools(cname, tempbools, numelems);
    out.reserve(numelems);
    //Move data to output vector
    for(ui bo = 0; bo < numelems; ++bo)
    {
        out.push_back(tempbools[bo]);
    }
    delete [] tempbools;
    return numelems;
}
#endif
