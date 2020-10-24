
add_library(Qt5::QICOPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Gui_install_prefix}/plugins/imageformats/qico.dll")
    _populate_Gui_plugin_properties(QICOPlugin RELEASE "imageformats/qico.dll" FALSE)
endif()
if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/imageformats/qicod.dll")
    _populate_Gui_plugin_properties(QICOPlugin DEBUG "imageformats/qicod.dll" FALSE)
endif()

list(APPEND Qt5Gui_PLUGINS Qt5::QICOPlugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_imageformats Qt5::QICOPlugin)
set_property(TARGET Qt5::QICOPlugin PROPERTY QT_PLUGIN_TYPE "imageformats")
set_property(TARGET Qt5::QICOPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QICOPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QICOPlugin")
