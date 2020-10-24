#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libzstd" for configuration "Debug"
set_property(TARGET libzstd APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libzstd PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/zstdd.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/zstdd.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS libzstd )
list(APPEND _IMPORT_CHECK_FILES_FOR_libzstd "${_IMPORT_PREFIX}/debug/lib/zstdd.lib" "${_IMPORT_PREFIX}/debug/bin/zstdd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
