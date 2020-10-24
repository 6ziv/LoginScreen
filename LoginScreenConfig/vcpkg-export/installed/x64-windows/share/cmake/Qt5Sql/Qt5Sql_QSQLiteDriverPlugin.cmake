
add_library(Qt5::QSQLiteDriverPlugin MODULE IMPORTED)


if(EXISTS "${_qt5Sql_install_prefix}/plugins/sqldrivers/qsqlite.dll")
    _populate_Sql_plugin_properties(QSQLiteDriverPlugin RELEASE "sqldrivers/qsqlite.dll" FALSE)
endif()
if(EXISTS "${_qt5Sql_install_prefix}/debug/plugins/sqldrivers/qsqlited.dll")
    _populate_Sql_plugin_properties(QSQLiteDriverPlugin DEBUG "sqldrivers/qsqlited.dll" FALSE)
endif()

list(APPEND Qt5Sql_PLUGINS Qt5::QSQLiteDriverPlugin)
set_property(TARGET Qt5::Sql APPEND PROPERTY QT_ALL_PLUGINS_sqldrivers Qt5::QSQLiteDriverPlugin)
set_property(TARGET Qt5::QSQLiteDriverPlugin PROPERTY QT_PLUGIN_TYPE "sqldrivers")
set_property(TARGET Qt5::QSQLiteDriverPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QSQLiteDriverPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QSQLiteDriverPlugin")
