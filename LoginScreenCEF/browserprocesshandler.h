#ifndef BROWSERPROCESSHANDLER_H
#define BROWSERPROCESSHANDLER_H
#include "cefheaders.h"
class BrowserProcessHandler : public CefBrowserProcessHandler
{
public:
    /*HANDLE rHandle;
    HANDLE wHandle;*/
    virtual void OnContextInitialized() OVERRIDE;
    IMPLEMENT_REFCOUNTING(BrowserProcessHandler);
};

#endif // BROWSERPROCESSHANDLER_H
