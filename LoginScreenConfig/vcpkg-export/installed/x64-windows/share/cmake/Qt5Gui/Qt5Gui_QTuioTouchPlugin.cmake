
add_library(Qt5::QTuioTouchPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Gui_install_prefix}/plugins/generic/qtuiotouchplugin.dll")
    _populate_Gui_plugin_properties(QTuioTouchPlugin RELEASE "generic/qtuiotouchplugin.dll" FALSE)
endif()
if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/generic/qtuiotouchplugind.dll")
    _populate_Gui_plugin_properties(QTuioTouchPlugin DEBUG "generic/qtuiotouchplugind.dll" FALSE)
endif()

list(APPEND Qt5Gui_PLUGINS Qt5::QTuioTouchPlugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_generic Qt5::QTuioTouchPlugin)
set_property(TARGET Qt5::QTuioTouchPlugin PROPERTY QT_PLUGIN_TYPE "generic")
set_property(TARGET Qt5::QTuioTouchPlugin PROPERTY QT_PLUGIN_EXTENDS "-")
set_property(TARGET Qt5::QTuioTouchPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QTuioTouchPlugin")
