// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <cstdio>
#include <cstring>

#ifndef MINIINI_NO_STL
#include <string>
#endif

#include "typedefs.h"
#include "globals.h"
#include "log.h"
#include "inisection.h"
#include "inifile.h"
#include "util.h"
#include "inifileutil.h"
#include "time.h"


using namespace miniini_private;

bool INIFile::OpenFile(const char * const fname)
{
    MINIINI_ASSERT(fname, "NULL pointer was passed as file name to"
                          "INIFile::OpenFile()");
    MINIINI_ASSERT(!IsValid(), "OpenFile() was called on an INIFile that is "
                               "already loaded");
    
    #ifdef MINIINI_BENCH_EXTRA
    #ifdef linux
    ld ftimestart = GetTime();
    #endif
    #endif

    //opening file
    FILE * const file = fopen( fname, "rb" );
    if(file == NULL)
    {
        MINIINI_ERROR("Couldn't open file. Maybe it does not exist? File: %s",
                      fname);
        return false;
    }
    //determining filesize
    i seek = fseek(file, 0, SEEK_END);
    if(seek != 0)
    {
        fclose(file);
        MINIINI_ERROR("Couldn't reach end of file. File: %s", fname);
        return false;
    }
    const i size = ftell(file);
    if(size == -1)
    {
        fclose(file);
        MINIINI_ERROR("Couldn't determine size of file. File: %s", fname);
        return false;
    }
    seek = fseek(file, 0, SEEK_SET);
    if(seek!=0)
    {
        fclose(file);
        MINIINI_ERROR("Couldn't reach start of file. File: %s", fname);
        return false;
    }
    //reading file to a buffer
    c * const buf = new c [size + 1];
    const i readnum = fread(buf, size, 1, file);
    fclose(file);

    #ifdef MINIINI_BENCH_EXTRA
    #ifdef linux
    bench_filetime = GetTime() - ftimestart;
    #endif
    #endif

    //if 0 blocks succesfully read
    if(readnum == 0)
    {
        delete [] buf;
        MINIINI_ERROR("Could open but could not read from file. File might be "
                      "corrupted or you might not have sufficient rights to "
                      "read from it. File: %s", fname);
        return false;
    }
    //adding trailing zero to the buffer
    buf[size] = 0;
    #ifdef MINIINI_BENCH_EXTRA
    #ifdef linux
    ld ltimestart = GetTime();
    #endif
    #endif
    bool out = LoadBuffer(buf, static_cast<unsigned>(size + 1));
    #ifdef MINIINI_BENCH_EXTRA
    #ifdef linux
    bench_loadtime = GetTime() - ltimestart;
    #endif
    #endif
    delete [] buf;
    return out;
}

bool INIFile::LoadBuffer(const char * buf, unsigned size)
{
    MINIINI_ASSERT(buf, "NULL pointer was passed as buffer to load from to"
                        "INIFile::LoadBuffer()");
    MINIINI_ASSERT(!IsValid(), "LoadBuffer() was called on an INIFile that is "
                               "already loaded");
    //Allocating memory for ini sections' strings in 16 blocks-
    //that results in a little speed overhead but potential memory
    //overhead of the allocator is decreased to a bit more than
    //1/16 of file size.
    Alloc = new Allocator(size, 16);
    //capacity of temporary buffer of pointers to sections
    ui tempsectionscap = 16;
    INISection::InitTempData();
    //temporary buffer of pointers to sections
    INISection * * tempsections = new INISection * [tempsectionscap];
    //ptr to the current character in buffer
    const c * currentchar = buf;
    //allocated capacity of headername
    ui headercap = 64;
    //buffer to load section header name to.
    c * headername = new c [headercap];
    //number of chars in headername
    ui headersize;
    //Loading the default INI section
    INISection * newsection = new INISection();
    newsection->Init("[DEFAULT]", &currentchar, Alloc);
    Insert(tempsections, Length, newsection);
    ++Length;
    //Iterating through lines in buffer, reading sections found
    while(*currentchar != '\0')
    {
        //Header() leaves currentchar at start of next line
        headersize = Header(currentchar, headername, headercap);
        //header found
        if(headersize)
        {
            //if temp section ptrs buffer runs out of space, reallocate it
            if(Length + 1 > tempsectionscap)
            {
                MINIINI_REALLOCATE(tempsections, tempsectionscap, Length, INISection *);
            }
            //Try to load new section
            newsection = new INISection();
            newsection->Init(headername, &currentchar, Alloc);
            //Insert new section
            if(Insert(tempsections, Length, newsection))
            {
                ++Length;
            }
            else
            {
                delete newsection;
            }
        }
    }
    delete [] headername;
    //Length is at least 1 due to the default section
    if(Length == 1)
    {
        MINIINI_WARNING("Empty INI file/buffer.");
    }
    //copy pointers from tempsections to Sections and delete tempsections
    Sections = new INISection * [Length];
    memcpy(Sections, tempsections, Length * sizeof(INISection *));
    delete [] tempsections;
    INISection::DestroyTempData();
    //Remove any unused memory blocks from the allocator.
    Alloc->Trim();
    return true;
}

INIFile::~INIFile()
{
    if(!IsValid())
    {
        return;
    }
    for(ui section = 0; section < Length; ++section)
    {
        delete Sections[section];
    }
    delete [] Sections;
    delete Alloc;
}

INISection * INIFile::GetSection(const char * const name) const
{
    MINIINI_ASSERT(name, "NULL pointer was passed as section name to"
                         "INIFile::GetSection()");
    i sectionidx; 
    //if an empty string ("") is passed, we're using the section that the 
    //iteration index points to
    if(*name == '\0')
    {
        MINIINI_ASSERT(Iter >= static_cast<i>(0) && Iter < static_cast<i>(Length), 
                       "Called INIFile::GetSection() with iteration index out"
                       "of range");
        sectionidx = Iter;
    }
    else
    {
        sectionidx = BinarySearch(Sections, Length, name);
    }
    if(sectionidx >= 0)
    {
        return Sections[sectionidx];
    }
    MINIINI_ERROR("Missing requested section. Section: %s", name);
    return NULL;
}

