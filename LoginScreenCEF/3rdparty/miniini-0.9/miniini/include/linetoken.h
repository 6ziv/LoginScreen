// Copyright (C) 2009-2010 Ferdinand Majerech
// This file is part of MiniINI
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef LINETOKEN_H_INCLUDED
#define LINETOKEN_H_INCLUDED

/// @cond PRIVATE
namespace miniini_private
{

///Specifies various parts of a line line in ini file during loading.
enum LineToken
{
    ///Name - this is where we are until we reach '=' or '[' character.
    LT_NAME,
    ///Value - this is where we are after we reach '=' character.
    LT_VAL,
    ///Header - this is where we are when we are between '[' and ']' characters.
    LT_HEADER
};

}
/// @endcond

#endif
