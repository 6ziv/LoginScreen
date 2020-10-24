#ifndef CONFIG_H
#define CONFIG_H
#include <QString>
#include <QCommandLineParser>
namespace Config
{
    void install();
    void uninstall();
    void activate();
    void deactivate();
    void resource(QString s);
    bool isinstalled();
    bool isactivated();
    QString resource();
    void restartAsAdmin(QString newcommand);
    inline bool parsing;
    inline QCommandLineParser parser;
};

#endif // CONFIG_H
