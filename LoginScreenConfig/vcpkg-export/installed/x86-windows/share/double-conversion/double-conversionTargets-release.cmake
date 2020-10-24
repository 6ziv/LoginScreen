#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "double-conversion::double-conversion" for configuration "Release"
set_property(TARGET double-conversion::double-conversion APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(double-conversion::double-conversion PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/double-conversion.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS double-conversion::double-conversion )
list(APPEND _IMPORT_CHECK_FILES_FOR_double-conversion::double-conversion "${_IMPORT_PREFIX}/lib/double-conversion.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
