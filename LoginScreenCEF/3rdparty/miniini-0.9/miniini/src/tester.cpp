// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <cstdio>
#include <cstring>
#include <cfloat>

#include <iostream>
#include <string>
#include <sstream>

#ifndef MINIINI_NO_STL
#include <vector>
#endif

#include "../../miniini.h"
#include "tester.h"

/// @cond PRIVATE
class TestData_INIFile
{
    protected:
        INIFile ini;
    
    public:
        TestData_INIFile()
            :ini()
        {
            if(!ini.OpenFile("test/test.ini"))
            {
                throw std::string("Could not open file test/test.ini");
            }
        }

        virtual ~TestData_INIFile(){};
};

TEST(TestData_INIFile, 1)
{
    SetName("Default section");
    INISection * section = ini.GetSection("[DEFAULT]");
    AssertTrue(section, "Default section was not loaded");
    AssertRString(section, "default", "default");
}

TEST(TestData_INIFile, 2)
{
    SetName("Section count");
    INISection * section = ini.GetSection("SETTINGS");
    AssertTrue(section, "[SETTINGS] was not loaded");
    AssertRInt(section, "sections", int(ini.GetLength()));
}

class TestData_Garbage : public TestData_INIFile
{
    protected:
        INISection * section;
    
    public:
        TestData_Garbage()
            :TestData_INIFile()
            ,section(NULL)
        {
            section = ini.GetSection("garbage");
            if(!section)
            {
                throw std::string("[garbage] was not loaded");
            }
        }

    private:
        TestData_Garbage(const TestData_Garbage &);
        void operator = (const TestData_Garbage &);
};

TEST(TestData_Garbage, 1)
{
    SetName("Tag count with garbage tags");
    AssertRInt(section, "numtags", int(section->GetLength()));
}

TEST(TestData_Garbage, 2)
{
    SetName("Spaces in tags and values");
    AssertRString(section, "spacesinaname", "spacesinaval");
}

TEST(TestData_Garbage, 3)
{
    SetName("Long tags and values");
    AssertRString(section, "aridiculouslylongname", 
                  "anevenmoreridiculouslylongvalueofabsolutelyawesometestingvalueforminiini");
}

class TestData_Iteration : public TestData_INIFile
{
    protected:
        INISection * section;
    
    public:
        TestData_Iteration()
            :TestData_INIFile()
            ,section(NULL)
        {
            section = ini.GetSection("iteration");
            if(!section)
            {
                throw std::string("[iteration] was not loaded");
            }
        }

    private:
        TestData_Iteration(const TestData_Iteration &);
        void operator = (const TestData_Iteration &);
};

TEST(TestData_Iteration, 1)
{
    SetName("Iteration over tags in a section");
    int count = 0;
    const char * valread;
    section->Reset();
    while(section->Next())
    {
        ++count;
        std::ostringstream tagstream;
        std::ostringstream valstream;
        tagstream << "tag" << count;
        valstream << "val" << count;
        AssertEqual(tagstream.str(), std::string(section->CurrentTag()),
                    "Unexpected tag name during iteration");
        section->ReadString("", valread);
        AssertEqual(valstream.str(), std::string(valread), "Unexpected tag value "
                    "during iteration");
    }
    AssertEqual(count, 4, "Incorrect number of iterations");
}

#ifndef MINIINI_NO_STL
TEST(TestData_Iteration, 2)
{
    SetName("Iteration over tags in a section, STL version");
    int count = 0;
    std::string valread;
    section->Reset();
    while(section->Next())
    {
        ++count;
        std::ostringstream tagstream;
        std::ostringstream valstream;
        tagstream << "tag" << count;
        valstream << "val" << count;
        AssertEqual(tagstream.str(), std::string(section->CurrentTagSTL()),
                    "Unexpected tag name during iteration");
        section->ReadString("", valread);
        AssertEqual(valstream.str(), valread, "Unexpected tag value "
                    "during iteration");
    }
    AssertEqual(count, 4, "Incorrect number of iterations");
}
#endif

class TestData_Values : public TestData_INIFile
{
    protected:
        INISection * section;
    
    public:
        TestData_Values()
            :TestData_INIFile()
            ,section(NULL)
        {
            section = ini.GetSection("vals");
            if(!section)
            {
                throw std::string("[vals] was not loaded");
            }
        }

    private:
        TestData_Values(const TestData_Values &);
        void operator = (const TestData_Values &);
};

TEST(TestData_Values, 1)
{
    SetName("Reading string that is not present");
    const char * readstring;
    if(section->ReadString("nothere", readstring))
    {
        throw std::string("Tag nothere= is read even though it's not present");
    }
}

TEST(TestData_Values, 2)
{
    SetName("Reading strings");
    AssertRString(section, "string", "string");
}

TEST(TestData_Values, 3)
{
    SetName("Reading integers");
    AssertRInt(section, "int", -5);
    AssertRInt(section, "intplus", 5);
    AssertRInt(section, "intmax", 2147483647);
    AssertRInt(section, "intmin", -2147483648);
    AssertRInt(section, "intover1", 2147483647);
    AssertRInt(section, "intover2", 2147483647);
}

TEST(TestData_Values, 4)
{
    SetName("Reading floats");
    AssertRFloat(section, "float", -1.545f);
    AssertRFloat(section, "float2", 0.5f);
    AssertRFloat(section, "float3", 0.5f);
    AssertRFloat(section, "float4", 5.0f);
    AssertRFloat(section, "floatplus", 2.3f);
    AssertRFloat(section, "floatover", FLT_MAX);
    AssertRFloat(section, "int", -5.0f);
}

TEST(TestData_Values, 5)
{
    SetName("Reading bools");
    AssertRBool(section, "bool", true);
    AssertRBool(section, "bool2", false);
    AssertRBool(section, "bool3", true);
    AssertRBool(section, "bool4", false);
}

#ifndef MINIINI_NO_STL
TEST(TestData_Values, 6)
{
    SetName("Reading strings, STL version");
    std::string readstring;
    if(!section->ReadString(std::string("string"), readstring))
    {
        throw std::string("Couldn't read string= from [vals]");
    }
    if(readstring != "string")
    {
        throw std::string("Read incorrect value from string= from [vals]\n"
                          "Expected value: string\nRead value: " + readstring);
    }
}

TEST(TestData_Values, 7)
{
    SetName("Reading integers, STL version");
    int readint;
    if(!section->ReadInt(std::string("int"), readint))
    {
        throw std::string("Couldn't read int= from [vals]");
    }
    if(readint != -5)
    {
        std::ostringstream error;
        error << "Read incorrect value from int= from [vals]\nExpected value: "
                 "-5\nRead value: " << readint;
        throw error.str();
    }
}

TEST(TestData_Values, 8)
{
    SetName("Reading floats, STL version");
    float readfloat;
    if(!section->ReadFloat(std::string("float"), readfloat))
    {
        throw std::string("Couldn't read float= from [vals]");
    }
    if(readfloat != -1.545f)
    {
        std::ostringstream error;
        error << "Read incorrect value from float= from [vals]\nExpected value: "
                 "-1.545\nRead value: " << readfloat;
        throw error.str();
    }
}

TEST(TestData_Values, 9)
{
    SetName("Reading booleans, STL version");
    bool readbool;
    if(!section->ReadBool(std::string("bool"), readbool))
    {
        throw std::string("Couldn't read bool= from [vals]");
    }
    if(readbool != true)
    {
        std::ostringstream error;
        error << "Read incorrect value from bool= from [vals]\nExpected value: "
                 "true\nRead value: " << readbool;
        throw error.str();
    }
}
#endif

class TestData_MultiValues : public TestData_INIFile
{
    protected:
        INISection * section;
    
    public:
        TestData_MultiValues()
            :TestData_INIFile()
            ,section(NULL)
        {
            section = ini.GetSection("multivals");
            if(!section)
            {
                throw std::string("[multivals] was not loaded");
            }
        }

    private:
        TestData_MultiValues(const TestData_MultiValues &);
        void operator = (const TestData_MultiValues &);
};

TEST(TestData_MultiValues, 1)
{
    SetName("Reading strings from a multi value tag");
    const char * readstrings[16];
    AssertRInt(section, "stringcount", 
               section->ReadMultiString("multistring", readstrings, 16));
    if(strcmp(readstrings[2], "strc"))
    {
        throw std::string("Read incorrect value/s from multistring= from "
                          "[multivals]");
    }
}

TEST(TestData_MultiValues, 2)
{
    SetName("Reading integers from a multi value tag");
    int readints[16];
    AssertRInt(section, "intcount", 
               section->ReadMultiInt("multiint", readints, 16));
}

TEST(TestData_MultiValues, 3)
{
    SetName("Reading floats from a multi value tag");
    float readfloats[16];
    AssertRInt(section, "floatcount", 
               section->ReadMultiFloat("multifloat", readfloats, 16));
}

TEST(TestData_MultiValues, 4)
{
    SetName("Reading booleans from a multi value tag");
    bool readbools[16];
    AssertRInt(section, "boolcount", 
               section->ReadMultiBool("multibool", readbools, 16));
}

#ifndef MINIINI_NO_STL
TEST(TestData_MultiValues, 5)
{
    SetName("Reading strings from a multi value tag, STL version");
    int stringcount;
    if(!section->ReadInt(std::string("stringcount"), stringcount))
    {
        throw "Couldn't read stringcount= from [multivals]";
    }
    std::vector<std::string> readstrings;
    AssertEqual(stringcount, int(section->ReadMultiString(
                std::string("multistring"), readstrings)),
                "Read unexpected value from stringcount="
                "in [multivals].");
}

TEST(TestData_MultiValues, 6)
{
    SetName("Reading integers from a multi value tag, STL version");
    int intcount;
    if(!section->ReadInt(std::string("intcount"), intcount))
    {
        throw "Couldn't read intcount= from [multivals]";
    }
    std::vector<int> readints;
    AssertEqual(intcount, int(section->ReadMultiInt(
                std::string("multiint"), readints)),
                "Read unexpected value from intcount="
                "in [multivals].");
}

TEST(TestData_MultiValues, 7)
{
    SetName("Reading floats from a multi value tag, STL version");
    int floatcount;
    if(!section->ReadInt(std::string("floatcount"), floatcount))
    {
        throw "Couldn't read floatcount= from [multivals]";
    }
    std::vector<float> readfloats;
    AssertEqual(floatcount, int(section->ReadMultiFloat(
                std::string("multifloat"), readfloats)),
                "Read unexpected value from floatcount="
                "in [multivals].");
}

TEST(TestData_MultiValues, 8)
{
    SetName("Reading booleans from a multi value tag, STL version");
    int boolcount;
    if(!section->ReadInt(std::string("boolcount"), boolcount))
    {
        throw "Couldn't read boolcount= from [multivals]";
    }
    std::vector<bool> readbools;
    AssertEqual(boolcount, int(section->ReadMultiBool(
                std::string("multibool"), readbools)),
                "Read unexpected value from boolcount="
                "in [multivals].");
}
#endif

class TestData_Arrays : public TestData_INIFile
{
    protected:
        INISection * section;
    
    public:
        TestData_Arrays()
            :TestData_INIFile()
            ,section(NULL)
        {
            section = ini.GetSection("arrays");
            if(!section)
            {
                throw std::string("[arrays] was not loaded");
            }
        }

    private:
        TestData_Arrays(const TestData_Arrays &);
        void operator = (const TestData_Arrays &);
};

TEST(TestData_Arrays, 1)
{
    SetName("Reading strings from a sequence of numbered tags");
    const char * readstrings[16];
    AssertRInt(section, "stringcount", 
               section->ReadStrings("string", readstrings, 16));
}

TEST(TestData_Arrays, 2)
{
    SetName("Reading integers from a sequence of numbered tags");
    int readints[16];
    AssertRInt(section, "intcount", 
               section->ReadInts("int", readints, 16));
}

TEST(TestData_Arrays, 3)
{
    SetName("Reading floats from a sequence of numbered tags");
    float readfloats[16];
    AssertRInt(section, "floatcount", 
               section->ReadFloats("float", readfloats, 16));
}

TEST(TestData_Arrays, 4)
{
    SetName("Reading bools from a sequence of numbered tags");
    bool readbools[16];
    AssertRInt(section, "boolcount", 
               section->ReadBools("bool", readbools, 16));
}

#ifndef MINIINI_NO_STL
TEST(TestData_Arrays, 5)
{
    SetName("Reading strings from a sequence of numbered tags, STL version");
    int stringcount;
    if(!section->ReadInt(std::string("stringcount"), stringcount))
    {
        throw "Couldn't read stringcount= from [arrays]";
    }
    std::vector<std::string> readstrings;
    AssertEqual(stringcount, int(section->ReadStrings(
                std::string("string"), readstrings)),
                "Read unexpected value from stringcount= in [arrays].");
}

TEST(TestData_Arrays, 6)
{
    SetName("Reading integers from a sequence of numbered tags, STL version");
    int intcount;
    if(!section->ReadInt(std::string("intcount"), intcount))
    {
        throw "Couldn't read intcount= from [arrays]";
    }
    std::vector<int> readints;
    AssertEqual(intcount, int(section->ReadInts(
                std::string("int"), readints)),
                "Read unexpected value from intcount= in [arrays].");
}

TEST(TestData_Arrays, 7)
{
    SetName("Reading floats from a sequence of numbered tags, STL version");
    int floatcount;
    if(!section->ReadInt(std::string("floatcount"), floatcount))
    {
        throw "Couldn't read floatcount= from [arrays]";
    }
    std::vector<float> readfloats;
    AssertEqual(floatcount, int(section->ReadFloats(
                std::string("float"), readfloats)),
                "Read unexpected value from floatcount= in [arrays].");
}

TEST(TestData_Arrays, 8)
{
    SetName("Reading booleans from a multi value tag, STL version");
    int boolcount;
    if(!section->ReadInt(std::string("boolcount"), boolcount))
    {
        throw "Couldn't read boolcount= from [arrays]";
    }
    std::vector<bool> readbools;
    AssertEqual(boolcount, int(section->ReadBools(
                std::string("bool"), readbools)),
                "Read unexpected value from boolcount="
                "in [arrays].");
}
#endif

void Log(const char * const log)
{
    std::cout << log << std::endl;
}

int main()
{
    INILogCallback(Log);
    bool failed = false;
    try
    {
        TestGroup<TestData_INIFile> inifilegroup("INIFile tests");
    }
    catch(TGroupFailed fail)
    {
        //If initialization of INIFile testgroup failed, we can't load test inifile
        if(fail == TGF_INIT)
        {
            return -2;
        }
        failed = true;
    }
    try{TestGroup<TestData_Garbage> garbagegroup("Garbage tests");}
    catch(TGroupFailed fail){failed = true;}
    try{TestGroup<TestData_Iteration> iterationgroup("Iteration tests");}
    catch(TGroupFailed fail){failed = true;}
    try{TestGroup<TestData_Values> valuesgroup("Reading simple values tests");}
    catch(TGroupFailed fail){failed = true;}
    try{TestGroup<TestData_MultiValues> mvaluesgroup("Reading multi values tests");}
    catch(TGroupFailed fail){failed = true;}
    try{TestGroup<TestData_Arrays> mvaluesgroup("Reading numbered tag sequences tests");}
    catch(TGroupFailed fail){failed = true;}
    if(failed)
    {
        return -1;
    }
    return 0;
}         
/// @endcond
