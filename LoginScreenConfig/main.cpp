#include "mainwindow.h"

#include <Windows.h>
#include <WinBase.h>
#include <Shlwapi.h>
#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDir>
#include <QSettings>

#include "config.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MessageBoxW(0,GetCommandLineW(),L"CMD",MB_OK);
    Config::parsing=true;
    Config::parser.addHelpOption();
    Config::parser.addOptions({
                          QCommandLineOption("install", "install the Credential Provider"),
                          QCommandLineOption("uninstall", "uninstall the Credential Provider"),
                          QCommandLineOption("activate", "register the Credential Provider"),
                          QCommandLineOption("deactivate", "unregister the Credential Provider"),
                          QCommandLineOption("resource", "set resource zip","resource"),
                          QCommandLineOption("batch", "run as batch, without window")
                      });

    Config::parser.process(a);
    if(Config::parser.isSet("install"))
        Config::install();
    if(Config::parser.isSet("activate"))
        Config::activate();
    if(Config::parser.isSet("resource"))
        Config::resource(Config::parser.value("resource"));
    if(Config::parser.isSet("deactivate"))
        Config::deactivate();
    if(Config::parser.isSet("uninstall"))
        Config::uninstall();
    if(Config::parser.isSet("batch"))
        return 0;
    Config::parsing=false;
    MainWindow w;
    w.show();
    return a.exec();
}
