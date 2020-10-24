// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef TESTER_H_INCLUDED
#define TESTER_H_INCLUDED

#include <cstring>
#include <string>
#include <iostream>
#include <sstream>

#include "../../miniini.h"

/// @cond PRIVATE

///Contains all data related to a single test
template <class Data>
class TestData : public Data
{   
    private:
        std::string __TestName__;

    public:
        
        TestData()
            :__TestName__()
        {}

        ///Dummy test method to be overridden
        template <int n>
        void Test()
        {
            throw std::string("");
        }

        void SetName(const std::string & name)
        {
            __TestName__ = name;
        }

        const std::string & GetName() const
        {
            return __TestName__;
        }

        static void AssertEqual(const char * in1, const char * in2, 
                                const std::string & errorstr)
        {
            if(strcmp(in1, in2))
            {
                throw errorstr;
            }
        }
        
        template<class T>
        static void AssertEqual(T in1, T in2, const std::string & errorstr)
        {
            if(!(in1 == in2))
            {
                throw errorstr;
            }
        }

        template<class T>
        static void AssertTrue(T in, const std::string & errorstr)
        {
            AssertEqual(static_cast<bool>(in), true, errorstr);
        }
        

        ////////////////////////////                          
        //MINIINI SPECIFIC ASSERTS//
        ////////////////////////////
        
        #define ASSERTREAD(section, tag, expectedvalue, readmethod, type)\
        {\
            type readvalue;\
            std::ostringstream error;\
            error << "Couldn't read " << tag << "= from [" << section->GetName()\
                  << "]";\
            AssertTrue(section->readmethod(tag.c_str(), readvalue), \
                       error.str());\
            error.str("");\
            error << "Read unexpected value from " << tag << "= from ["\
                  << section->GetName() << "]\nExpected value: " <<\
                  expectedvalue << "\nRead value: " << readvalue;\
            AssertEqual(readvalue, expectedvalue, error.str());\
        }

        static void AssertRString(INISection * section, const std::string & tag,
                                  const std::string & expectedvalue)
        {
            ASSERTREAD(section, tag, expectedvalue.c_str(), ReadString, 
                       const char *);
        }
        
        static void AssertRInt(INISection * section, const std::string & tag, 
                               int expectedvalue)
        {
            ASSERTREAD(section, tag, expectedvalue, ReadInt, int);
        }
        
        static void AssertRFloat(INISection * section, const std::string & tag,
                                 float expectedvalue)
        {
            ASSERTREAD(section, tag, expectedvalue, ReadFloat, float);
        }

        static void AssertRBool(INISection * section, const std::string & tag,
                                bool expectedvalue)
        {
            ASSERTREAD(section, tag, expectedvalue, ReadBool, bool);
        }

        #undef ASSERTREAD
};

enum TGroupFailed
{
    TGF_INIT,
    TGF_DESTROY,
    TGF_TEST
};

///Handles run of a single test group
template<class Data, int MaxTests=64>
class TestGroup
{
    public:

        typedef TestData<Data> DataObject;

    private:
        
        ///Generates dummy test methods
        template <class Test, class Group, int methods>
        struct TestRegisterer
        {
            static void Register(Group& group)
            {
                group.Register(methods, &Test::template Test<methods>);
                TestRegisterer<Test, Group, methods - 1>::Register(group);
            }
        };

        template <class Test, class Group>
        struct TestRegisterer<Test, Group, 0>
        {
            static void Register(Group&)
            {
            }
        };


        typedef void (DataObject::*TestMethod)();
       
        ///Name of the test group
        std::string Name;

        ///Data common for all tests in this group.
        DataObject * GroupData; 
        
        ///Function pointers to tests
        TestMethod Tests [MaxTests];

    public:

        ///Constructor. Executes the test group and cleans it up. Throws a const char* in case of failure.
        TestGroup(const std::string & name)
            :Name(name)
            ,GroupData(NULL)
        {    
            //Generate and register all test methods
            TestRegisterer<DataObject, TestGroup, MaxTests>::Register(*this);
            std::cout << "\n\nRunning test group: " << Name << "\n";

            //Initialize test data
            std::cout << "\nInitializing test data\n";
            try
            {
                GroupData = new DataObject;
            }
            catch(const std::string & errorstr)
            {
                std::cout << "Initialization FAILED\nCause: " << errorstr << "\n";
                throw TGF_INIT;
            }
            
            //Total number of valid (not dummy) tests in the group
            unsigned tests = 0;
            //Number of failed tests in the group
            unsigned failedtests = 0;

            //Run tests
            std::cout << "\nRunning tests\n\n";
            for(unsigned test = 0; test < MaxTests; ++test)
            {
                try
                {
                    (GroupData->*Tests[test])();
                    std::cout << "Test successful: " << GroupData->GetName() 
                              << "\n";
                    ++tests;
                }
                catch(const std::string & errorstr)
                {
                    //Dummy tests throw an empty string
                    if(!errorstr.empty())
                    {
                        std::cout << "Test FAILED: " << GroupData->GetName() <<
                                     "\nCause: " << errorstr << "\n";
                        ++tests;
                        ++failedtests;
                    }
                }
            }

            //Destroy test data
            std::cout << "\nDestroying test data\n";
            try
            {
                delete GroupData;
            }
            catch(const std::string & errorstr)
            {
                std::cout << "Destruction FAILED\nCause: " << errorstr << "\n";
                throw TGF_DESTROY;
            }

            std::cout << "\nTests:\nTotal: " << tests << "\nFailed: " << 
                         failedtests << "\n";

            //If any test failed, the test group failed as well.
            if(failedtests)
            {
                std::cout << "Test group FAILED\n";
                throw TGF_TEST;
            }

        }

        void Register(int n, TestMethod method)
        {
            Tests[n - 1] = method;
        }

    private:

        TestGroup(const TestGroup &);
        
        void operator = (const TestGroup &);
};

#define TEST(data, number)\
    template<>\
    template<>\
    void TestGroup<data>::DataObject::Test<number>()

#endif
/// @endcond
