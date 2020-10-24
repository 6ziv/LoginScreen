// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef INIFILE_H_INCLUDED
#define INIFILE_H_INCLUDED

#include "typedefs.h"
#include "globals.h"
#include "allocator.h"
#include "inisection.h"
#include "miniini_assert.h"

/**
 * @mainpage MiniINI API Documentation
 *
 * @section intro Introduction
 *
 * This is the complete API documentation for MiniINI. It describes all classes, 
 * methods and global functions provided by MiniINI. This is not the best place
 * to start learning how to use MiniINI. Rather, it should serve as reference
 * to support you when you need more detailed information than found in the
 * tutorials. You can find these in the doc/ directory in MiniINI package and in
 * the documentation section of 
 * <a href=http://miniini.tuxfamily.org>MiniINI website</a>.
 */


///Reads data from an INI file and gives access to its sections.
class INIFile
{
    private:

        ///Number of sections in the file. Includes default section so it's at least 1 after INIFile is initialized.
        miniini_private::ui Length;
        ///Array of pointers to sections.
        INISection * * Sections;
        ///Allocator for string data of this file.
        miniini_private::Allocator * Alloc;
        ///Iteration index.
        miniini_private::i Iter;


    public:

        ///Empty constructor.
        INIFile()
            :Length(0)
            ,Sections(NULL)
            ,Alloc(NULL)
            ,Iter(-1)
        {}

        ///Resets iteration.
        void Reset()
        {
            Iter = -1;
        }

        /** Advances iteration to the next section.
         * @return true if there is another section and iteration moved to it.
         * @return false if there are no more sections, i.e. we're finished with iteration.
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

        /** Loads data from an INI file.
		 * @note You can only call OpenFile on an uninitialized INIFile.
         * @note File will be loaded even if it's empty, and even empty sections will be read.
         * @param fname Name of the file to load. Must be a zero terminated string.
         * @return true if the file was successfully loaded.
         * @return false if the file couldn't be loaded.
         */
        bool OpenFile(const char * const fname);

        /** Loads data from an INI file (STL version).
		 * @note You can only call OpenFile on an uninitialized INIFile.
         * @note File will be loaded even if it's empty, and even empty sections will be read.
         * @param fname Name of the file to load.
         * @return true if the file is successfully loaded.
         * @return false if the file couldn't be loaded.
         */
        #ifndef MINIINI_NO_STL
        bool OpenFile (const std::string & fname)
        {
            return OpenFile(fname.c_str());
        }
        #endif

        /** Loads data from a buffer.
		 * @note You can only call LoadBuffer on an uninitialized INIFile.
         * @note Given buffer @b must end with a zero character.
         * @note Buffer will be loaded even if it's empty, and even empty sections will be read.
         * Useful when loading from archives.
         * @param buf Buffer to load from. Must be zero terminated.
         * @param size Size of the buffer in bytes (including terminating zero).
		 * @return true if successfully loaded.
		 * @return false if there was an error and the buffer was not loaded.
         */
        bool LoadBuffer(const char * buf, unsigned size);

        /** Destructor.
         * Deletes loaded INI data (if any).
         */
        ~INIFile();

        /** Gets pointer a section.
         * @param name Name of the section to get. Must be a zero terminated string.
         * @return pointer to the section if it exists.
         * @return NULL if the section doesn't exist.
         */
        INISection * GetSection(const char * const name) const;

        /** Gets pointer to a section (STL version).
         * @param name Name of the section to get.
         * @return pointer to the section if it exists.
         * @return NULL if the section doesn't exist.
         */
        #ifndef MINIINI_NO_STL
        INISection * GetSection(const std::string & name) const
        {
            return GetSection(name.c_str());
        }
        #endif

        /** [] operator : alias for GetSection().
         * @param name Name of the section to get. Must be a zero terminated string.
         * @return pointer to the section if it exists.
         * @return NULL if the section doesn't exist.
         */
        INISection * operator [] (const char * const name) const
        {
            MINIINI_ASSERT(name, "NULL pointer was passed as section name to"
                                 "INIFile::operator []");
            return GetSection(name);
        }

        /** [] operator : alias for GetSection() (STL version).
         * @param name Name of the section to get.
         * @return pointer to the section if it exists.
         * @return NULL if the section doesn't exist.
         */
        #ifndef MINIINI_NO_STL
        INISection * operator [] (const std::string & name) const
        {
            return GetSection(name);
        }
        #endif
        
        /** Gets number of sections in the file.
         * @return number of sections in the file.
         */
        unsigned GetLength() const
        {
            return static_cast<unsigned>(Length);
        }

        /** Determines if the INIFile is initialized.
         * @return true if this INIFile is initialized.
         * @return false if this INIFile is not initialized.
         */
        bool IsValid() const
        {
            //If any of these is 0/NULL, this INIFile was constructed
            //but not initialised. (Length will be at least 1 after 
            //initialization due to default section)
            return Length && Sections && Alloc;
        }

    private:

        /** Tries to read a header from line starting by given character pointer.
         * Updates character pointer to point to next line.
         * If needed, reallocates header buffer and updates its pointer and capacity.
         * @param currentcharref Reference to pointer to current character in a buffer.
         * @param header Reference to buffer where to read the header name (if any) to.
         * @param headercap Reference to the capacity of header buffer.
         * @return size of header or 0 if there is no header in the line or its size is 0 (which is invalid)
         */
        static inline miniini_private::ui Header(const miniini_private::c * & currentcharref, 
                                                 miniini_private::c * & header, 
                                                 miniini_private::ui & headercap);

        INIFile(const INIFile &);

        void operator = (const INIFile &);
};

#endif
