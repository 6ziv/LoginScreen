
add_library(Qt5::QGenericEnginePlugin MODULE IMPORTED)


if(EXISTS "${_qt5Network_install_prefix}/plugins/bearer/qgenericbearer.dll")
    _populate_Network_plugin_properties(QGenericEnginePlugin RELEASE "bearer/qgenericbearer.dll" FALSE)
endif()
if(EXISTS "${_qt5Network_install_prefix}/debug/plugins/bearer/qgenericbearerd.dll")
    _populate_Network_plugin_properties(QGenericEnginePlugin DEBUG "bearer/qgenericbearerd.dll" FALSE)
endif()

list(APPEND Qt5Network_PLUGINS Qt5::QGenericEnginePlugin)
set_property(TARGET Qt5::Network APPEND PROPERTY QT_ALL_PLUGINS_bearer Qt5::QGenericEnginePlugin)
set_property(TARGET Qt5::QGenericEnginePlugin PROPERTY QT_PLUGIN_TYPE "bearer")
set_property(TARGET Qt5::QGenericEnginePlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QGenericEnginePlugin PROPERTY QT_PLUGIN_CLASS_NAME "QGenericEnginePlugin")
