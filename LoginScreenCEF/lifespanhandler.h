#ifndef LIFESPANHANDLER_H
#define LIFESPANHANDLER_H
#include "cefheaders.h"

class LifespanHandler :public CefLifeSpanHandler
{
public:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    IMPLEMENT_REFCOUNTING(LifespanHandler);
};

#endif // LIFESPANHANDLER_H
