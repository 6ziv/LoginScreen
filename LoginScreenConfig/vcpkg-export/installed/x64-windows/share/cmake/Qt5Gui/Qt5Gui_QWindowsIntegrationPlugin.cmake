
add_library(Qt5::QWindowsIntegrationPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Gui_install_prefix}/plugins/platforms/qwindows.dll")
    _populate_Gui_plugin_properties(QWindowsIntegrationPlugin RELEASE "platforms/qwindows.dll" FALSE)
endif()
if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/platforms/qwindowsd.dll")
    _populate_Gui_plugin_properties(QWindowsIntegrationPlugin DEBUG "platforms/qwindowsd.dll" FALSE)
endif()

list(APPEND Qt5Gui_PLUGINS Qt5::QWindowsIntegrationPlugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_platforms Qt5::QWindowsIntegrationPlugin)
set_property(TARGET Qt5::QWindowsIntegrationPlugin PROPERTY QT_PLUGIN_TYPE "platforms")
set_property(TARGET Qt5::QWindowsIntegrationPlugin PROPERTY QT_PLUGIN_EXTENDS "-")
set_property(TARGET Qt5::QWindowsIntegrationPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QWindowsIntegrationPlugin")
