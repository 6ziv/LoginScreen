
add_library(Qt5::QODBCDriverPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Sql_install_prefix}/plugins/sqldrivers/qsqlodbc.dll")
    _populate_Sql_plugin_properties(QODBCDriverPlugin RELEASE "sqldrivers/qsqlodbc.dll" FALSE)
endif()
if(EXISTS "${_qt5Sql_install_prefix}/debug/plugins/sqldrivers/qsqlodbcd.dll")
    _populate_Sql_plugin_properties(QODBCDriverPlugin DEBUG "sqldrivers/qsqlodbcd.dll" FALSE)
endif()

list(APPEND Qt5Sql_PLUGINS Qt5::QODBCDriverPlugin)
set_property(TARGET Qt5::Sql APPEND PROPERTY QT_ALL_PLUGINS_sqldrivers Qt5::QODBCDriverPlugin)
set_property(TARGET Qt5::QODBCDriverPlugin PROPERTY QT_PLUGIN_TYPE "sqldrivers")
set_property(TARGET Qt5::QODBCDriverPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QODBCDriverPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QODBCDriverPlugin")
