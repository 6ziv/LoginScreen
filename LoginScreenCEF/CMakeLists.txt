cmake_minimum_required(VERSION 3.12)


project(LoginScreenCEF LANGUAGES CXX)
cmake_policy(SET CMP0077 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VCPKG_TARGET_TRIPLET "x64-windows-static")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg-export/scripts/buildsystems/vcpkg.cmake")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cef_binary_80.0.8+gf96cd1d+chromium-80.0.3987.132_windows64/cmake)

set(LIBZIP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg-export/installed/x64-windows-static/share/libzip)
set(ZLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg-export/installed/x64-windows-static)
set(BZip2_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg-export/installed/x64-windows-static)
set(CEF_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cef_binary_80.0.8+gf96cd1d+chromium-80.0.3987.132_windows64)
set(PATH_SUFFIXES ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg-export/installed/)

find_package(LIBZIP REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

set(USE_SANDBOX OFF)
set(USE_ATL OFF)
find_package(CEF REQUIRED)
list(APPEND CMAKE_CXX_FLAGS /EHsc)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cef_binary_80.0.8+gf96cd1d+chromium-80.0.3987.132_windows64")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg-export/installed/x64-windows-static/include")


add_subdirectory(${CEF_ROOT}/libcef_dll libcef_dll_wrapper)
ADD_LOGICAL_TARGET(libcef_lib ${CEF_LIB_DEBUG} ${CEF_LIB_RELEASE})
add_executable(LoginScreenCEF app.cpp avatarloader.cpp browserprocesshandler.cpp client.cpp conf.cpp contentloader.cpp contextmenuhandler.cpp cookieaccessfilter.cpp displayhandler.cpp lifespanhandler.cpp loadhandler.cpp main.cpp mimeguess.cpp requesthandler.cpp resourcehandler.cpp resourcerequesthandler.cpp usernames.cpp
    app.h avatarloader.h browserprocesshandler.h cefheaders.h client.h conf.h contentloader.h contextmenuhandler.h cookieaccessfilter.h displayhandler.h helpers.h lifespanhandler.h
    loadhandler.h Login.h mimeguess.h requesthandler.h resourcehandler.h resourcerequesthandler.h stringstream.h usernames.h
    )
target_link_libraries(LoginScreenCEF PRIVATE libzip::zip)

target_link_libraries(LoginScreenCEF PRIVATE libcef_lib libcef_dll_wrapper)
target_link_libraries(LoginScreenCEF PRIVATE Shlwapi)
install(TARGETS LoginScreenCEF DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(TARGETS libcef_dll_wrapper DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cef_binary_80.0.8+gf96cd1d+chromium-80.0.3987.132_windows64/Resources/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cef_binary_80.0.8+gf96cd1d+chromium-80.0.3987.132_windows64/Release/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
