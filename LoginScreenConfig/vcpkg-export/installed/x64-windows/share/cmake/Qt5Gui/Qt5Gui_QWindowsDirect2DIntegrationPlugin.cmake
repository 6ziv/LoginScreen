
add_library(Qt5::QWindowsDirect2DIntegrationPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Gui_install_prefix}/plugins/platforms/qdirect2d.dll")
    _populate_Gui_plugin_properties(QWindowsDirect2DIntegrationPlugin RELEASE "platforms/qdirect2d.dll" FALSE)
endif()
if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/platforms/qdirect2dd.dll")
    _populate_Gui_plugin_properties(QWindowsDirect2DIntegrationPlugin DEBUG "platforms/qdirect2dd.dll" FALSE)
endif()

list(APPEND Qt5Gui_PLUGINS Qt5::QWindowsDirect2DIntegrationPlugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_platforms Qt5::QWindowsDirect2DIntegrationPlugin)
set_property(TARGET Qt5::QWindowsDirect2DIntegrationPlugin PROPERTY QT_PLUGIN_TYPE "platforms")
set_property(TARGET Qt5::QWindowsDirect2DIntegrationPlugin PROPERTY QT_PLUGIN_EXTENDS "-")
set_property(TARGET Qt5::QWindowsDirect2DIntegrationPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QWindowsDirect2DIntegrationPlugin")
