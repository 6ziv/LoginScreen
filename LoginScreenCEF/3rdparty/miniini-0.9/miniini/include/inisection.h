// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef INISECTION_H_INCLUDED
#define INISECTION_H_INCLUDED

#include <cstring>
#include <cstdlib>

#include "typedefs.h"
#include "miniini_assert.h"
#include "allocator.h"
#include "linetoken.h"
#ifndef MINIINI_NO_STL
#include <string>
#include <vector>
#endif



/** Handles reading of data from a single INI section.
 */
class INISection
{
    friend class INIFile;
    private:

        ///Name of the section. Read from section header.
        miniini_private::c * Name;
        ///Number of tags in the section.
        miniini_private::ui Length;
        /** Tags in the section.
         * Each tag is stored as a char buffer containing name, a trailing
         * zero, any number of values followed by trailing zeroes, with the
         * last value followed by two trailing zeroes.
         */
        miniini_private::c * * Tags;
        ///Allocator for strings in the section.
        miniini_private::Allocator * Alloc;
        ///Iteration index
        miniini_private::i Iter;

                              
        ///Data that doesn't need to be reinitialized in every constructor call.
        ///Temp tags buffer capacity.
        static miniini_private::ui temptagscap;
        ///Stores tags during loading.
        static miniini_private::c * * temptags;
        ///Tag buffer capacity.
        static miniini_private::ui tagcap;
        ///Stores currently processed tag during loading.
        static miniini_private::c * tagbuf;

    public:

        /** Gets name of this section.
         * @return name of this section.
         */
        const char * GetName() const
        {
            return Name;
        }

        #ifndef MINIINI_NO_STL
        /** Gets name of this section (STL version).
         * @return name of this section.
         */
        std::string GetNameSTL() const
        {
            return std::string(Name);
        }
        #endif

        /** Gets number of tags in the section.
         * @return number of tags in the section.
         */
        unsigned GetLength() const
        {
            return static_cast<unsigned>(Length);
        }

        ///Resets iteration
        void Reset()
        {
            Iter = -1;
        }

        /** Advance iteration to the next tag.
         * @return true if there is another tag and iteration moved to it.
         * @return false if there are no more tags, i.e. we're finished with iteration.
         */
        bool Next()
        {
            if(Iter < static_cast<miniini_private::i>(Length) - 1)
            {
                ++Iter;
                return true;
            }
            return false;
        }
        /** Get name of the current tag.
         * @return name of the tag currently pointed to by internal iteration index.
         */
        const char * CurrentTag() const
        {
            MINIINI_ASSERT(Iter >= static_cast<miniini_private::i>(0) && 
                           Iter < static_cast<miniini_private::i>(Length), 
                           "Called INISection::CurrentTag() with iteration index "
                           "out of range");
            return Tags[Iter];
        }

        /** Get name of the current tag (STL version).
         * @return name of the tag currently pointed to by internal iteration index.
         */
        #ifndef MINIINI_NO_STL
        std::string CurrentTagSTL() const
        {
            MINIINI_ASSERT(Iter >= static_cast<miniini_private::i>(0) && 
                           Iter < static_cast<miniini_private::i>(Length), 
                           "Called INISection::CurrentTagSTL() with iteration "
                           "index out of range");
            return std::string(Tags[Iter]);
        }
        #endif

        /** Read a string from a tag.
         * @note Unlike other ReadXXX methods, ReadString only returns a pointer
         * to INISection data. If you want to use this data after INIFile is 
         * destroyed, you have to copy it.
         * @note If the tag has multiple values, the first value is read.
         * @param name Name of the tag to read. Must be a zero terminated string.
         * @param out Reference to variable to read to. If tag is not found, out won't be changed.
         * @return true if the value was read successfully.
         * @return false if the tag does not exist and no value was read. 
         */
        bool ReadString(const char * const name, const char * & out) const;

        /** Read a string from a tag (STL version).
         * @note If the tag has multiple values, the first value is read.
         * @param name Name of the tag to read.
         * @param out Reference to variable to read to. If tag is not found, out won't be changed.
         * @return true if the value was read successfully.
         * @return false if the tag does not exist and no value was read. 
         */
        #ifndef MINIINI_NO_STL
        bool ReadString(const std::string & name, std::string & out) const
        {
            const miniini_private::c * tempout;
            bool ret = ReadString(name.c_str(), tempout);
            if(ret)
            {
                out = tempout;
            }
            return ret;
        }
        #endif

        /** Read an int from a tag.
         * @note If the tag has multiple values, the first value is read.
         * @param name Name of the tag to read. Must be a zero terminated string.
         * @param out Reference to variable to read to. If tag is not found, out won't be changed.
         * @return true if the value was read successfully.
         * @return false if the tag does not exist and no value was read. 
         */
        bool ReadInt(const char * const name, int & out) const;

        /** Read an int from a tag (STL version).
         * @note If the tag has multiple values, the first value is read.
         * @param name Name of the tag to read.
         * @param out Reference to variable to read to. If tag is not found, out won't be changed.
         * @return true if the value was read successfully.
         * @return false if the tag does not exist and no value was read. 
         */
        #ifndef MINIINI_NO_STL
        bool ReadInt(const std::string & name, int & out) const
        {
            return ReadInt(name.c_str(), out);
        }
        #endif

        /** Read a float from a tag.
         * @note If the tag has multiple values, the first value is read.
         * @param name Name of the tag to read. Must be a zero terminated string.
         * @param out Reference to variable to read to. If tag is not found, out won't be changed.
         * @return true if the value was read successfully
         * @return false if the tag does not exist and no value was read.
         */
        bool ReadFloat(const char * const name, float & out) const;

        /** Read a float from a tag (STL version).
         * @note If the tag has multiple values, the first value is read.
         * @param name Name of the tag to read.
         * @param out Reference to variable to read to. If tag is not found, out won't be changed.
         * @return true if the value was read successfully
         * @return false if the tag does not exist and no value was read.
         */
        #ifndef MINIINI_NO_STL
        bool ReadFloat(const std::string & name, float & out) const
        {
            return ReadFloat(name.c_str(), out);
        }
        #endif

        /** Read a bool from a tag.
         * @note If the tag has multiple values, the first value is read.
         * @param name Name of the tag to read. Must be a zero terminated string.
         * @param out Reference to variable to read to. If tag is not found, out won't be changed.
         * @return true if the value was read successfully
         * @return false if the tag does not exist and no value was read.
         */
        bool ReadBool(const char * const name, bool & out) const;

        /** Read a bool from a tag (STL version).
         * @note If the tag has multiple values, the first value is read.
         * @param name Name of the tag to read.
         * @param out Reference to variable to read to. If tag is not found, out won't be changed.
         * @return true if the value was read successfully
         * @return false if the tag does not exist and no value was read.
         */
        #ifndef MINIINI_NO_STL
        bool ReadBool(const std::string & name, bool & out) const
        {
            return ReadBool(name.c_str(), out);
        }
        #endif

        /** Determine number of values in a tag.
         * @param name Name of tag. Must be a zero terminated string.
         * @return number of values in the tag.
         */
        unsigned MultiValSize(const char * const name) const;

        /** Read an array of strings from a multi value tag.
         * Reads strings from comma separated values in a tag.
         * @note Unlike other ReadMultiXXX methods, ReadMultiString only returns 
         * a pointer to INISection data. If you want to use this data after 
         * INIFile is destroyed, you have to copy it.
         * @param name Name of the tag to read. Must be a zero terminated string.
         * @param out Pointer to buffer to read to.
         * @param cap Maximum number of values the buffer can hold.
         * @return number of values read.
         */
        unsigned ReadMultiString(const char * const name, const char * * out,
                                 const unsigned cap) const;

        /** Read an array of strings from a multi value tag (STL version).
         * Reads strings from comma separated values in a tag.
         * @note Values are added to the end of vector.
         * @param name Name of the tag to read.
         * @param out Reference to the vector to read to.
         * @return number of values read.
         */
        #ifndef MINIINI_NO_STL
        unsigned ReadMultiString(const std::string & name,
                                 std::vector<std::string> & out) const;
        #endif

        /** Read an array of ints from a multi value tag.
         * Reads ints from comma separated values in a tag.
         * @param name Name of the tag to read. Must be a zero terminated string.
         * @param out Pointer to buffer to read to.
         * @param cap Maximum number of values the buffer can hold.
         * @return number of values read.
         */
        unsigned ReadMultiInt(const char * const name, int * out,
                              const unsigned cap) const;

        /** Read an array of ints from a multi value tag (STL version).
         * Reads ints from comma separated values in a tag.
         * @note Values are added to the end of vector.
         * @param name Name of the tag to read.
         * @param out Reference to the vector to read to.
         * @return number of values read.
         */
        #ifndef MINIINI_NO_STL
        unsigned ReadMultiInt(const std::string & name,
                              std::vector<int> & out) const;
        #endif

        /** Read an array of floats from a multi value tag.
         * Reads floats from comma separated values in a tag.
         * @param name Name of the tag to read. Must be a zero terminated string.
         * @param out Pointer to buffer to read to.
         * @param cap Maximum number of values the buffer can hold.
         * @return number of values read.
         */
        unsigned ReadMultiFloat(const char * const name, float * out,
                                const unsigned cap) const;

        /** Read an array of floats from a multi value tag (STL version).
         * Reads floats from comma separated values in a tag.
         * @note Values are added to the end of vector.
         * @param name Name of the tag to read.
         * @param out Reference to the vector to read to.
         * @return number of values read.
         */
        #ifndef MINIINI_NO_STL
        unsigned ReadMultiFloat(const std::string & name,
                                std::vector<float> & out) const;
        #endif

        /** Read an array of bools from a multi value tag.
         * Reads bools from comma separated values in a tag.
         * @param name Name of the tag to read. Must be a zero terminated string.
         * @param out Pointer to buffer to read to.
         * @param cap Maximum number of values the buffer can hold.
         * @return number of values read.
         */
        unsigned ReadMultiBool(const char * const name, bool * out,
                               const unsigned cap) const;

        /** Read an array of bools from a multi value tag (STL version).
         * Reads bools from comma separated values in a tag.
         * @note Values are added to the end of vector.
         * @param name Name of the tag to read.
         * @param out Reference to the vector to read to.
         * @return number of values read.
         */
        #ifndef MINIINI_NO_STL
        unsigned ReadMultiBool(const std::string & name,
                               std::vector<bool> & out) const;
        #endif

        /** Determine size of a sequence of tags with given base name.
         * Counts tags named name1=, name2=, etc. where name is the
         * given base name for the tags.
         * @param name Base name of tags. Must be a zero terminated string.
         * @return number of elements in the array.
         */
        unsigned ArraySize(const char * const name) const;

        /** Read an array of strings from a sequence of tags with given base name.
         * Reads values from tags named name1=, name2=, etc. where name is the
         * given base name for the tags.
         * @note If any tag has multiple values, the first value is read.
         * @note Unlike other ReadXXXs methods, ReadStrings only returns a
         * pointer to INISection data. If you want to use this data after 
         * INIFile is destroyed, you have to copy it.
         * @note After a call to ReadStrings, contents of out after and 
         * including out[return value] are undefined.
         * @param name Base name of tags to read. Must be a zero terminated string.
         * @param out Pointer to buffer to read to.
         * @param cap Maximum number of values the buffer can hold.
         * @return number of values read.
         */
        unsigned ReadStrings(const char * const name, const char * * out,
                             const unsigned cap) const;

        /** Read an array of strings from a sequence of tags with given base name (STL version).
         * Reads values from tags named name1=, name2=, etc. where name is the
         * given base name for the tags. 
         * @note If any tag has multiple values, the first value is read.
         * @note Values are added to the end of vector.
         * @param name Base name of tags to read.
         * @param out Reference to the vector to read to.
         * @return number of values read.
         */
        #ifndef MINIINI_NO_STL
        unsigned ReadStrings(const std::string & name,
                             std::vector<std::string> & out) const;
        #endif

        /** Read an array of ints from a sequence of tags with given base name.
         * Reads values from tags named name1=, name2=, etc. where name is the
         * given base name for the tags.
         * @note If any tag has multiple values, the first value is read.
         * @param name Base name of tags to read. Must be a zero terminated string.
         * @param out Pointer to buffer to read to.
         * @param cap Maximum number of values the buffer can hold.
         * @return number of values read.
         */
        unsigned ReadInts(const char * const name, int * out,
                          const unsigned cap) const;

        /** Read an array of ints from a sequence of tags with given base name (STL version).
         * Reads values from tags named name1=, name2=, etc. where name is the
         * given base name for the tags. 
         * @note If any tag has multiple values, the first value is read.
         * @note Values are added to the end of vector.
         * @param name Base name of tags to read.
         * @param out Reference to the vector to read to.
         * @return number of values read.
         */
        #ifndef MINIINI_NO_STL
        unsigned ReadInts(const std::string & name,
                          std::vector<int> & out) const;
        #endif

        /** Read an array of floats from a sequence of tags with given base name.
         * Reads values from tags named name1=, name2=, etc. where name is the
         * given base name for the tags.
         * @note If any tag has multiple values, the first value is read.
         * @param name Base name of tags to read. Must be a zero terminated string.
         * @param out Pointer to buffer to read to.
         * @param cap Maximum number of values the buffer can hold.
         * @return number of values read
         */
        unsigned ReadFloats(const char * const name, float * out,
                            const unsigned cap) const;

        /** Read an array of floats from a sequence of tags with given base name (STL version).
         * Reads values from tags named name1=, name2=, etc. where name is the
         * given base name for the tags. 
         * @note If any tag has multiple values, the first value is read.
         * @note Values are added to the end of vector.
         * @param name Base name of tags to read.
         * @param out Reference to the vector to read to.
         * @return number of values read
         */
        #ifndef MINIINI_NO_STL
        unsigned ReadFloats(const std::string & name,
                            std::vector<float> & out) const;
        #endif

        /** Read an array of bools from a sequence of tags with given base name.
         * Reads values from tags named name1=, name2=, etc. where name is the
         * given base name for the tags.
         * @note If any tag has multiple values, the first value is read.
         * @param name Base name of tags to read. Must be a zero terminated string.
         * @param out Pointer to buffer to read to.
         * @param cap Maximum number of values the buffer can hold.
         * @return number of values read
         */
        unsigned ReadBools(const char * const name, bool * out,
                           const unsigned cap) const;

        /** Read an array of bools from a sequence of tags with given base name (STL version).
         * Reads values from tags named name1=, name2=, etc. where name is the
         * given base name for the tags. 
         * @note If any tag has multiple values, the first value is read.
         * @note Values are added to the end of vector.
         * @param name Base name of tags to read.
         * @param out Reference to the vector to read to.
         * @return number of values read
         */
        #ifndef MINIINI_NO_STL
        unsigned ReadBools(const std::string & name,
                           std::vector<bool> & out) const;
        #endif

    private:

        ///Empty constructor.
        INISection()
            :Name(NULL)
            ,Length(0)
            ,Tags(NULL)
            ,Alloc(NULL)
            ,Iter(-1)
        {}

        ///Destructor.
        ~INISection();

        /** Tries to read a tag name from line starting by given character pointer.
         * Updates character pointer and tag size according to data processed.
         * Tag name is read to static member tagbuf, which is reallocated if needed.
         * @param currentcharref Reference to pointer to current character in a buffer.
         * @param tagsize Reference to size of the tag.
         * @return LT_VAL if this is a valid tag with a value.
         * Current char ptr is updated to point to the first character after the '='
         * and tag size is updated to number of characters read to tagbuf.
         * @return LT_HEADER if this line contains a header. Current char ptr is updated
         * to point to '[', NOT after it.
         * @return LT_NAME otherwise. In this case the line contains nothing interesting
         * and we can ignore it. Current char ptr is updated to point to start of next
         * line.
         */
        static inline miniini_private::LineToken TagName(const miniini_private::c * & currentcharref,
                                                         miniini_private::ui & tagsizeref);

        /** Tries to read a tag value from line starting by given character pointer.
         * Updates character pointer to the start of the next line or end of the buffer,
         * and tag size according to data processed.
         * Tag value is read to static member tagbuf, which is reallocated if needed.
         * @param currentcharref Reference to pointer to current character in a buffer.
         * @param tagsize Reference to size of the tag.
         * @return size of tag buffer after value is read.
         */
        static inline miniini_private::ui TagValue(const miniini_private::c * & currentcharref,
                                                   miniini_private::ui tagsize);

        /** Determines if the line staring by given character pointer contains a header.
         * @param currentcharref Reference to pointer to current character in a buffer.
         * @returns true if header is found and not empty.
         * @returns false otherwise, and updates current char pointer to the next line.
         */
        static inline bool Header(const miniini_private::c * & currentcharref);

        /** Parses given strings as ints.
         * Will stop parsing when a non-int string is encountered.
         * @param strings Strings to parse.
         * @param out Buffer to output ints to.
         * @param numstrings Number of strings.
         * @return Number of parsed strings.
         */
        static inline miniini_private::ui ParseInts(const miniini_private::c * * strings,
                                                    int * out,
                                                    const miniini_private::ui numstrings);

        /** Parses given strings as floats.
         * Will stop parsing when a non-float string is encountered.
         * @param strings Strings to parse.
         * @param out Buffer to output floats to.
         * @param numstrings Number of strings.
         * @return Number of parsed strings.
         */
        static inline miniini_private::ui ParseFloats(const miniini_private::c * * strings,
                                                      float * out,
                                                      const miniini_private::ui numstrings);

        /** Parses given strings as bools.
         * Will stop parsing when a non-bool string is encountered.
         * @param strings Strings to parse.
         * @param out Buffer to output bools to.
         * @param numstrings Number of strings.
         * @return Number of parsed strings.
         */
        static inline miniini_private::ui ParseBools(const miniini_private::c * * strings,
                                                     bool * out,
                                                     const miniini_private::ui numstrings);

        /** Loads the section from given position in a raw ini file buffer.
         * Loads the section and changes currentcharptr so that the caller can
         * start with another section.
         * @param sectionname Name of the section to initialize
         * @param currentcharptr Start of the section in a raw ini file buffer.
         * @return true if the section is valid.
         * @return false if the section is empty.
         */
        void Init(const miniini_private::c * const sectionname,
                  const miniini_private::c * * const currentcharptr,
                  miniini_private::Allocator * const alloc);

        ///Allocates/initializes static data
        static void InitTempData()
        {
            MINIINI_ASSERT(!temptags, "INISection::InitTempData() : temptags "
                                     "is already allocated");
            MINIINI_ASSERT(!tagbuf, "INISection::InitTempData() : tagbuf is "
                                   "already allocated");
            temptagscap = 8;
            temptags = new miniini_private::c * [temptagscap];
            //MUST be over 6
            tagcap = 64;
            tagbuf = new miniini_private::c [tagcap];
        }

        ///Destroys static data
        static void DestroyTempData()
        {
            MINIINI_ASSERT(temptags, "INISection::DestroyTempData() : temptags "
                                     "is not allocated");
            MINIINI_ASSERT(tagbuf, "INISection::DestroyTempData() : tagbuf is "
                                   "not allocated");
            temptagscap = 0;
            delete [] temptags;
            temptags = NULL;
            tagcap = 0;
            delete [] tagbuf;
            tagbuf = NULL;
        }

        ///No assignment or copyconstruction.

        INISection(const INISection &);

        void operator = (const INISection &);
};

#endif
