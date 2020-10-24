#include <cstdio>
#include <cstring>

#ifndef MINIINI_NO_STL
#include <string>
#include <vector>
#endif

#include "../../miniini.h"
#include "globals.h"
#include "time.h"

/// @cond PRIVATE

///Type of benchmark to perform. Self-explanatory.
enum BenchmarkType
{
    BT_STRING,
    BT_INT,
    BT_FLOAT,
    BT_BOOL,
    BT_MULTISTRING,
    BT_MULTIINT,
    BT_MULTIFLOAT,
    BT_MULTIBOOL,
    BT_STRINGS,
    BT_INTS,
    BT_FLOATS,
    BT_BOOLS,

    BT_STRINGRULES,

    BT_NONE
};

///Performs benchmarks of INI reading functionality
class ReadMark
{
    private:

        ///Type of benchmark
        BenchmarkType Type;

        ///Are we benchmarking STL API?
        bool STL;

        ///Input filename for the benchmark
        char * BenchFile;

        ///Section we're currently reading
        INISection * CurrentSection;


    public:

        ///Time it took to read (just read data from, not process it in any way) the benchmarked file
        long double FileTime;

        ///Time it took to allocate all blocks by Allocator.
        long double AllocTime;

        ///Time it took to load all INI data (INIFile::LoadBuffer)
        long double LoadTime;

        ///Constructor. Determines type of benchmark from its name
        ReadMark(char * benchname, bool stl)
            :Type(BT_NONE)
            ,STL(stl)
            ,BenchFile(NULL)
            ,CurrentSection(NULL)
            ,FileTime(0.0)
            ,AllocTime(0.0)
            ,LoadTime(0.0)
        {
            #define SETTYPE(str, type, file)\
                if(!strcmp(benchname, str))\
                {\
                    size_t fnamelen = strlen(file) + 1;\
                    BenchFile = new char [fnamelen];\
                    Type = type;\
                    memcpy(BenchFile, file, fnamelen);\
                };
            SETTYPE("readstring", BT_STRING, "test/benchmark/string.ini");
            SETTYPE("readint", BT_INT, "test/benchmark/int.ini");
            SETTYPE("readfloat", BT_FLOAT, "test/benchmark/float.ini");
            SETTYPE("readbool", BT_BOOL, "test/benchmark/bool.ini");
            SETTYPE("readstringm", BT_MULTISTRING, "test/benchmark/stringm.ini");
            SETTYPE("readintm", BT_MULTIINT, "test/benchmark/intm.ini");
            SETTYPE("readfloatm", BT_MULTIFLOAT, "test/benchmark/floatm.ini");
            SETTYPE("readboolm", BT_MULTIBOOL, "test/benchmark/boolm.ini");
            SETTYPE("readstrings", BT_STRINGS, "test/benchmark/strings.ini");
            SETTYPE("readints", BT_INTS, "test/benchmark/ints.ini");
            SETTYPE("readfloats", BT_FLOATS, "test/benchmark/floats.ini");
            SETTYPE("readbools", BT_BOOLS, "test/benchmark/bools.ini");
            SETTYPE("readstringrules", BT_STRINGRULES, "test/benchmark/rulesmd2.ini");
            #undef SETTYPE
        }

        ~ReadMark()
        {
            delete [] BenchFile;
        }

        //ReadXXX benchmarks over single section
        template<typename T, typename T2>
        void BenchRead(bool (INISection::*Read)(T, T2&) const)
        {
            while(CurrentSection->Next())
            {
                T2 out;
                (CurrentSection->*Read)(CurrentSection->CurrentTag(), out);
            }
        }

        ///ReadMultiXXX benchmarks over single section.
        ///Can't handle more than 1024 values in one tag.
        #ifndef MINIINI_NO_STL
        #define BENCHREADMULTI(method, type, stltype)\
        while(CurrentSection->Next())\
        {\
            if(STL)\
            {\
                std::vector<stltype> out;\
                CurrentSection->method(CurrentSection->CurrentTag(), out);\
            }\
            else\
            {\
                unsigned elemcount = 1024;\
                type * out = new type [elemcount];\
                CurrentSection->method(CurrentSection->CurrentTag(), out, elemcount);\
                delete [] out;\
            }\
        }
        #else
        #define BENCHREADMULTI(method, type, stltype)\
        while(CurrentSection->Next())\
        {\
            unsigned elemcount = 1024;\
            type * out = new type [elemcount];\
            CurrentSection->method(CurrentSection->CurrentTag(), out, elemcount);\
            delete [] out;\
        }
        #endif

        #ifndef MINIINI_NO_STL
        /** ReadXXXs benchmarks over single section.
         * These require specific inifiles. Each section has to contain tags a=,
         * b= , etc. that specify lengths of arrays with same base name, e.g.
         * a=2 means there's going to be a1= and a2=.
         */
        #define BENCHREADARRAY(method, type, stltype)\
        for(char tagname [] = "a"; tagname[0] <= 'z'; ++(tagname[0]))\
        {\
            if(STL)\
            {\
                std::vector<stltype> out;\
                CurrentSection->method(std::string(tagname), out);\
            }\
            else\
            {\
                int elemcount;\
                if(!CurrentSection->ReadInt(tagname, elemcount))\
                {\
                    break;\
                }\
                type * out = new type [elemcount];\
                CurrentSection->method(tagname, out, elemcount);\
                delete [] out;\
            }\
        }
        #else
        #define BENCHREADARRAY(method, type, stltype)\
        for(char tagname [] = "a"; tagname[0] <= 'z'; ++(tagname[0]))\
        {\
            int elemcount;\
            if(!CurrentSection->ReadInt(tagname, elemcount))\
            {\
                break;\
            }\
            type * out = new type [elemcount];\
            CurrentSection->method(tagname, out, elemcount);\
            delete [] out;\
        }
        #endif

        ///Runs the benchmark
        bool Benchmark()
        {
            INIFile ini;
            if(!ini.OpenFile(BenchFile)) return false;
            while(ini.Next())
            {
                CurrentSection = ini.GetSection(ini.GetSection("")->GetName());
                switch(Type)
                {
                    case BT_STRING:
                    case BT_STRINGRULES:
                        if(!STL) BenchRead<const char * const, const char *>(&INISection::ReadString);
                        #ifndef MINIINI_NO_STL
                        else BenchRead<const std::string &, std::string>(&INISection::ReadString);
                        #endif
                        break;
                    case BT_INT:
                        if(!STL) BenchRead<const char * const, int>(&INISection::ReadInt);
                        #ifndef MINIINI_NO_STL
                        else BenchRead<const std::string &, int>(&INISection::ReadInt);
                        #endif
                        break;
                    case BT_FLOAT:
                        if(!STL) BenchRead<const char * const, float>(&INISection::ReadFloat);
                        #ifndef MINIINI_NO_STL
                        else BenchRead<const std::string &, float>(&INISection::ReadFloat);
                        #endif
                        break;
                    case BT_BOOL:
                        if(!STL) BenchRead<const char * const, bool>(&INISection::ReadBool);
                        #ifndef MINIINI_NO_STL
                        else BenchRead<const std::string &, bool>(&INISection::ReadBool);
                        #endif
                        break;
                    case BT_MULTISTRING:
                        BENCHREADMULTI(ReadMultiString, const char *, std::string);
                        break;
                    case BT_MULTIINT:
                        BENCHREADMULTI(ReadMultiInt, int, int);
                        break;
                    case BT_MULTIFLOAT:
                        BENCHREADMULTI(ReadMultiFloat, float, float);
                        break;
                    case BT_MULTIBOOL:
                        BENCHREADMULTI(ReadMultiBool, bool, bool);
                        break;
                    case BT_STRINGS:
                        BENCHREADARRAY(ReadStrings, const char *, std::string);
                        break;
                    case BT_INTS:
                        BENCHREADARRAY(ReadInts, int, int);
                        break;
                    case BT_FLOATS:
                        BENCHREADARRAY(ReadFloats, float, float);
                    case BT_BOOLS:
                        BENCHREADARRAY(ReadBools, bool, bool);
                        break;
                    default:
                        return false;
                        break;
                }

            }

            #ifdef MINIINI_BENCH_EXTRA
            #ifdef linux
            FileTime = miniini_private::bench_filetime;
            AllocTime = miniini_private::bench_alloctime;
            LoadTime = miniini_private::bench_loadtime;
            #endif
            #endif

            return true;
        }

        #undef BENCHREADARRAY
        #undef BENCHREADMULTI


    private:

        ReadMark(const ReadMark &);

        void operator = (const ReadMark &);
};

void help()
{
    puts
    (
        "MiniINI benchmark\n"
        "Copyright (C) 2009-2010 Ferdinand Majerech\n"
        "Usage: bench [OPTIONS] [BENCHMARKS]\n"
        "Requires files to be present in test/benchmark for benchmarks to run\n"
        "Benchmarks and files they require:\n"
        " readstring         string.ini\n"
        " readint            int.ini\n"
        " readfloat          float.ini\n"
        " readbool           bool.ini\n"
        " readstringm        stringm.ini\n"
        " readintm           intm.ini\n"
        " readfloatm         floatm.ini\n"
        " readboolm          boolm.ini\n"
        " readstrings        strings.ini\n"
        " readints           ints.ini\n"
        " readfloats         floats.ini\n"
        " readbools          bools.ini\n"
        " readstringrules    rulesmd2.ini\n"
        "Options:\n"
        "    --stl           STL API will be benchmarked instead of cstdlib API\n"
        "                    (for benchmarks where this applies)\n"
        #ifdef MINIINI_BENCH_EXTRA
        #ifdef linux
        "    --extra         Output extra benchmarking data\n"
        #endif
        #endif
    );
}

int main(int argc, char * argv [])
{
    if(argc > 1)
    {
        #ifdef MINIINI_BENCH_EXTRA
        #ifdef linux
        long double timestart = miniini_private::GetTime();
        long double filetime = 0.0;
        long double alloctime = 0.0;
        long double loadtime = 0.0;
        #endif
        #endif

        bool needhelp = true;
        bool stl = false;
        bool extra = false;
        for(int arg = 1; arg < argc; ++arg)
        {
            if(!strcmp(argv[arg], "--stl"))
            {
                stl = true;
                #ifdef MINIINI_NO_STL
                stl = false;
                #endif
            }
            else
            if(!strcmp(argv[arg], "--extra"))
            {
                extra = true;
            }
            else
            {
                ReadMark bench(argv[arg], stl);
                if(!bench.Benchmark())
                {
                    printf("ERROR in benchmark %s\n", argv[arg]);
                    return -1;
                }
                #ifdef MINIINI_BENCH_EXTRA
                #ifdef linux
                filetime += bench.FileTime;
                alloctime += bench.AllocTime;
                loadtime += bench.LoadTime;
                #endif
                #endif
                needhelp = false;
            }
        }
        if(needhelp)
        {
            help();
            return -1;
        }
        #ifdef MINIINI_BENCH_EXTRA
        #ifdef linux
        long double totaltime = miniini_private::GetTime() - timestart;
        if(extra)
        {
            printf("TESTUTIL Total_time_ms = %Lf\nTESTUTIL File_time_ms = %Lf\n"
                   "TESTUTIL File_time_ratio = %Lf\nTESTUTIL Alloc_time_ms = %Lf\n"
                   "TESTUTIL Alloc_time_ratio = %Lf\nTESTUTIL Load_time_ms = %Lf\n"
                   "TESTUTIL Load_time_ratio = %Lf\n",
                   totaltime * 1000.0, filetime * 1000.0,
                   filetime / totaltime , alloctime * 1000.0,
                   alloctime / totaltime , loadtime * 1000.0,
                   loadtime / totaltime );
        }
        #endif
        #endif
    }
    else
    {
        help();
        return -4;
    }
    return 0;
}

/// @endcond
