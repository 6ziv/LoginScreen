MiniINI version 0.9.0


== Introduction ==

MiniINI is a free/open source, minimalistic, easy to use C++ library for reading
INI (aka CFG) files. It has no dependencies other than the standard library and
should compile on any platform with a standards compliant compiler with C99 
support. (only a makefile for GCC is included at the moment, though)

Main priority of MiniINI is, as its name suggests, minimalism and speed. Its 
goal is to create fastest INI parser possible without sacrificing ease of use. 
MiniINI should be especially useful for game development, for instance game 
settings, properties of units in strategy games, etc. MiniINI is already very
fast. It takes under 50 ms to load, parse and unload an INI file about 1MB large
with 512 sections according to benchmarks. This is on a ~1.7 GHz dual-core 
machine (although MiniINI can only use one core).

MiniINI can also check files it reads, and issue warnings for the most common 
mistakes in ini code. This should be useful mainly for users of programs based 
on MiniINI, for instance game modders. 

At the moment, MiniINI can only read INI files, not write to them. This should 
change in future versions.

You can get newest versions of MiniINI at miniini.tuxfamily.org .

=== INI file format ===

INI files are text files containing simple values formatted as tag=value pairs,
for example:
```
answer=42
```
These are grouped in sections which start by headers containing names in square
brackets, for example:
```
[section]
```
INI files can contain comments, which are ignored. Comments usually start by a
semicolon (default in MiniINI) or hash and continue to the end of line. Example:
```
tag=value ;comment
```
A simple complete INI file example:
```
[General]
OS=Linux
Version=0.4
Renderers=OpenGLRenderer, SWRenderer
CurrentRenderer=OpenGLRenderer

[OpenGLRenderer]
ScreenWidth=1024
ScreenHeight=768
UseShaders=true
```

== Features ==

==== INI parsing ====
- Reads from INI/CFG files, or from user provided buffer containing contents 
  of an INI/CFG file. This allows the user e.g. to load data from a compressed 
  file and pass it to MiniINI.
- Can read headerless ini/cfg files.
- Only plain ASCII files are supported. There is no support for Unicode, and 
  there probably never will be.
- Case-sensitive. That means that [CASE], [case] and [Case] are not the same 
  and there is a difference between Tag= and TAG=
- Ignores *all* spaces, i.e. no spaces/tabs in tags or values. For example:    +
                  +
  tag=125685      +
  and             +
  t a g = 125 685 +
                  +
  both have the same meaning. Spaces _might_ be supported in the future for values, 
  if there will be a need, but are not planned at the moment.
- Can read arrays of data from tags with multiple, comma separated values.
- Can also read arrays from numbered sequences of tags, for example:
                  +
                  +
  a1=1            +
  a2=2            +
  a3=3            +
                  +
- Cannot write to ini files at this time. This should be implemented in future.


==== Interface ====
- Provides methods to read strings, ints, floats and bools from inifile
  and checks the ini data for errors, allowing the programmer to use
  their own default values.
- Supports both plain strings and arrays and STL strings and vectors. STL
  functionality can be disabled for more minimalism.
- Supports iteration over contents of INI files.


==== Configurability ====
- Supports single line comments with a configurable comment character. So if you
  want to use __'#'__ instead of __';'__ , you can. There is no support for 
  multiple comment characters at the same time for performance reasons.
- Name=value separator is configurable as well, __'='__ by default.


==== Documentation ====
- Tutorials detailing every feature of the library.
- Maintained API documentation.


==== Performance ====
- Extremely fast. Even multi-MB ini files with hundreds or thousands of sections
  can be processed in fractions of a second.
- Low memory usage. Data structure overhead is reduced as much as possible.


==== Debugging ====
- Debug build can issue warnings to the user using a callback function 
  specified by the programmer. For example when a tag from which the program 
  tries to load an integer contains something else.


== New in this release ==

There are almost no changes to the interface in MiniINI 0.9 . However, all 
testing and benchmarking code was rewritten, some library code was refactored,
and there was a focus on improving documentation.

Tutorials were rewritten to improve readability. The API documentation has also
seen heavy changes. Many bugs were fixed and descriptions should now be more,
well, descriptive.

Allocator, INIFile and logging code was refactored.

Regression testing code was completely rewritten, and should be more 
maintainable now.

Benchmarking code was rewritten, separated from MiniINI code and more 
benchmarking functionality was added.

All testing and benchmarking scripts were rewritten in Python 3.1 , and again
the new code should be more maintainable.

There were also some small bugfixes. See CHANGES.txt for more details.


== Compiling/Installing/Using ==


=== Directory structure ===
MiniINI files are split into multiple directories:
```
 ./         (top-level directory) MiniINI include file and this readme.

 ./miniini  MiniINI source code        

 ./bin      Compiled binary files, e.g. the library itself.

 ./doc      Documentation.

 ./example  Example programs.

 ./test     Files for regression tester and benchmark scripts.
```  
      
=== Requirements ===
MiniINI should compile on any standards compliant C++ compiler, and requires no
third-party libraries. Basic C99 support is also required. If you have gcc, 
there is a GNU makefile tested with gcc 4.x , which should also work with older
gcc versions.

MiniINI also includes benchmarking scripts, which require Python and Valgrind to
run. These scripts are not necessary to compile or use MiniINI and are used for
development of MiniINI.

=== Compiling/Installing with gcc ===
Type __make__ in terminal.
You can now link to compiled libraries (in ./bin) directly, or, if you're on
a Linux/Unix system, you can install MiniINI by typing __make install__ as a
root user (e.g. __sudo make install__ if you have sudo) This will compile and
copy optimized build to __/usr/lib/libminiini.a__ , debug build to 
__/usr/lib/libminiini-dbg.a__ , and MiniINI headers to __/usr/include/__.

Debug and optimized builds can also be compiled separately using __make debug__
and __make optimized__ .

You can uninstall MiniINI by typing __make uninstall__ as a root user.

=== Using MiniINI ===
First, you will need to include __miniini.h__ header file and link with either
debug or optimized build. To include MiniINI, add this line to your code:
```
#include<miniini.h>
```
To link with MiniINI using GCC, add this to your compiler options:
```
-lminiini 
```
for optimized or
```
-lminiini-dbg 
```
for debug build.

If you didn't install MiniINI, you'll also need to add path with __miniini.h__ 
to header paths of your compiler and path with __libminiini.a__ and/or 
__libminiini-dbg.a__ to linker paths.

For more info you can check [tutorials and API documentation doc/index.html] 
in __./doc__ directory and examples in __./example__ directory.

== License ==

MiniINI is free/open source software distributed under the MIT/X11 license. This
license allows you to use MiniINI in both open and closed source software, free
of charge, and modify it if you need to.

Full text of the license:

```
Copyright (c) 2009-2010 Ferdinand Majerech

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
```

== Contact/Credits ==
MiniINI homepage: miniini.tuxfamily.org

MiniINI Launchpad page: https://launchpad.net/miniini

MiniINI was created by Ferdinand Majerech aka Kiith-Sa kiithsacmp[AT]gmail.com

MiniINI was created using Vim and GCC on Ubuntu Linux, as an INI parsing 
library for Catom https://launchpad.net/catom .

--------------------------------------------------------------------------------

**%%mtime(Last update: %d-%m-%Y)**
