
add_library(Qt5::QPSQLDriverPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Sql_install_prefix}/plugins/sqldrivers/qsqlpsql.dll")
    _populate_Sql_plugin_properties(QPSQLDriverPlugin RELEASE "sqldrivers/qsqlpsql.dll" FALSE)
endif()
if(EXISTS "${_qt5Sql_install_prefix}/debug/plugins/sqldrivers/qsqlpsqld.dll")
    _populate_Sql_plugin_properties(QPSQLDriverPlugin DEBUG "sqldrivers/qsqlpsqld.dll" FALSE)
endif()

list(APPEND Qt5Sql_PLUGINS Qt5::QPSQLDriverPlugin)
set_property(TARGET Qt5::Sql APPEND PROPERTY QT_ALL_PLUGINS_sqldrivers Qt5::QPSQLDriverPlugin)
set_property(TARGET Qt5::QPSQLDriverPlugin PROPERTY QT_PLUGIN_TYPE "sqldrivers")
set_property(TARGET Qt5::QPSQLDriverPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QPSQLDriverPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QPSQLDriverPlugin")
