#ifndef CONF_H
#define CONF_H
#include "cefheaders.h"
class Conf
{
public:
    static void InitSettings(CefSettings &);
    static void InitBrowserSettings(CefBrowserSettings &);
    static std::wstring getResourceFile();
};

#endif // CONF_H
