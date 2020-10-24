cmake_minimum_required(VERSION 3.5)
project(LoginScreenConfig LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
cmake_policy(SET CMP0087 NEW)
add_executable(LoginScreenConfig
    main.cpp
    mainwindow.cpp
    mainwindow.h
    config.cpp
    config.h
    priv.cpp
    priv.h
)
target_link_libraries(LoginScreenConfig PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(LoginScreenConfig "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

install(TARGETS LoginScreenConfig DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(CODE
    "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E
        env PATH=\"${_qt_bin_dir}\" \"${WINDEPLOYQT_EXECUTABLE}\"
            --verbose 0
            --no-compiler-runtime
            \"${CMAKE_INSTALL_PREFIX}/bin/LoginScreenConfig.exe\")"
        )

    install(CODE
        "execute_process(COMMAND \"${CMAKE_INSTALL_PREFIX}/bin/LoginScreenConfig.exe\" --install --activate \"--resource=${CMAKE_INSTALL_PREFIX}/sample.zip\" --batch
        )")
