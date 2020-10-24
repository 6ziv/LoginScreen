
add_library(Qt5::QWindowsPrinterSupportPlugin MODULE IMPORTED)


if(EXISTS "${_qt5PrintSupport_install_prefix}/plugins/printsupport/windowsprintersupport.dll")
    _populate_PrintSupport_plugin_properties(QWindowsPrinterSupportPlugin RELEASE "printsupport/windowsprintersupport.dll" FALSE)
endif()
if(EXISTS "${_qt5PrintSupport_install_prefix}/debug/plugins/printsupport/windowsprintersupportd.dll")
    _populate_PrintSupport_plugin_properties(QWindowsPrinterSupportPlugin DEBUG "printsupport/windowsprintersupportd.dll" FALSE)
endif()

list(APPEND Qt5PrintSupport_PLUGINS Qt5::QWindowsPrinterSupportPlugin)
set_property(TARGET Qt5::PrintSupport APPEND PROPERTY QT_ALL_PLUGINS_printsupport Qt5::QWindowsPrinterSupportPlugin)
set_property(TARGET Qt5::QWindowsPrinterSupportPlugin PROPERTY QT_PLUGIN_TYPE "printsupport")
set_property(TARGET Qt5::QWindowsPrinterSupportPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QWindowsPrinterSupportPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QWindowsPrinterSupportPlugin")
