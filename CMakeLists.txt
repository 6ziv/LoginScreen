cmake_minimum_required(VERSION 3.12)

project(LoginScreen LANGUAGES CXX)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/LoginScreenCEF ${CMAKE_BINARY_DIR}/LoginScreenCEF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/LoginScreenConfig ${CMAKE_BINARY_DIR}/LoginScreenConfig)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/LoginScreenLibrary ${CMAKE_BINARY_DIR}/LoginScreenLibrary)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/LoginScreenInstall ${CMAKE_BINARY_DIR}/LoginScreenInstall)


