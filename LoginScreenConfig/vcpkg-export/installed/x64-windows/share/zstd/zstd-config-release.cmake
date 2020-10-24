#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libzstd" for configuration "Release"
set_property(TARGET libzstd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libzstd PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/zstd.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/zstd.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS libzstd )
list(APPEND _IMPORT_CHECK_FILES_FOR_libzstd "${_IMPORT_PREFIX}/lib/zstd.lib" "${_IMPORT_PREFIX}/bin/zstd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
