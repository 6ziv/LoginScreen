cmake_minimum_required(VERSION 3.5)

project(LoginScreenLibrary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LINK_DEF_FILE_FLAG "/def:")
set(LSL_INC ClassFactory.h CSampleCredential.h CSampleProvider.h DllWrapper.h guid.h helpers.h RefCounter.h)
set(LSL_SRC CSampleCredential.cpp CSampleProvider.cpp Dll.cpp DllWrapper.cpp helpers.cpp LoginScreenLibrary.def)
add_library(LoginScreenLibrary SHARED ${LSL_INC} ${LSL_SRC})

list(APPEND CEF_COMPILER_FLAGS /EHsc /W4)
list(APPEND CMAKE_CXX_FLAGS /EHsc /W4)

target_link_libraries(LoginScreenLibrary PRIVATE Credui.lib Shlwapi.lib Secur32.lib advapi32.lib ole32.lib)
install(TARGETS LoginScreenLibrary DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
