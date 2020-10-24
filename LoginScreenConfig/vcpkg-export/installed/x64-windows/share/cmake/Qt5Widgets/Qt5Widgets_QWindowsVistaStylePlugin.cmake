
add_library(Qt5::QWindowsVistaStylePlugin MODULE IMPORTED)


if(EXISTS "${_qt5Widgets_install_prefix}/plugins/styles/qwindowsvistastyle.dll")
    _populate_Widgets_plugin_properties(QWindowsVistaStylePlugin RELEASE "styles/qwindowsvistastyle.dll" FALSE)
endif()
if(EXISTS "${_qt5Widgets_install_prefix}/debug/plugins/styles/qwindowsvistastyled.dll")
    _populate_Widgets_plugin_properties(QWindowsVistaStylePlugin DEBUG "styles/qwindowsvistastyled.dll" FALSE)
endif()

list(APPEND Qt5Widgets_PLUGINS Qt5::QWindowsVistaStylePlugin)
set_property(TARGET Qt5::Widgets APPEND PROPERTY QT_ALL_PLUGINS_styles Qt5::QWindowsVistaStylePlugin)
set_property(TARGET Qt5::QWindowsVistaStylePlugin PROPERTY QT_PLUGIN_TYPE "styles")
set_property(TARGET Qt5::QWindowsVistaStylePlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QWindowsVistaStylePlugin PROPERTY QT_PLUGIN_CLASS_NAME "QWindowsVistaStylePlugin")
