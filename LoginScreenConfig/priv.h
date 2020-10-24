#ifndef PRIV_H
#define PRIV_H
#include <QString>
namespace Priv
{
    bool IsAdmin();
    bool RunAsAdmin(QString arg);
};

#endif // PRIV_H
