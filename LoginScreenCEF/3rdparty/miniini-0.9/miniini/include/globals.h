// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include "typedefs.h"

/// @cond PRIVATE
namespace miniini_private
{

#ifdef MINIINI_BENCH_EXTRA
#ifdef linux
///Used for high resolution file read time measurement.
extern miniini_private::ld bench_filetime;
///Used for high resolution allocator block allocation time measurement.
extern miniini_private::ld bench_alloctime;
///Used for high resolution INI loading time measurement.
extern miniini_private::ld bench_loadtime;
#endif
#endif

///Single line comment character. ';' by default. Can be defined by user.
extern c comment;                                                   

///Separator char between tag and value. '=' by default. Can be defined by user.
extern c namevalsep;

/** Characters parsed as line separators
 * CR, LF, trailing zero
 */
extern c linesep[3];

}
/// @endcond

/** Sets the comment character.
 * Default comment character is ';'. This function changes it to given 
 * character. Only a character that does not serve a special purpose in INI 
 * files can be set as the comment character.
 * @param comment_char New comment character.
 * @return true if comment character was set successfully.
 * @return false if comment could not be set to this character.
 */
bool INISetComment(const char comment_char);

/** Sets name=value separator character.
 * Default separator is '='. This function changes it to given character.
 * Only a character that does not serve a special purpose in INI files can be
 * set as the separator.
 * @param comment_char New separator.
 * @return true if separator was set successfully.
 * @return false if separator could not be set to this character.
 */
bool INISetSeparator(const char sep);
#endif
