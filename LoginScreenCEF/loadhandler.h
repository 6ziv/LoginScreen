#ifndef LOADHANDLER_H
#define LOADHANDLER_H

#include "cefheaders.h"
class LoadHandler : public CefLoadHandler
{
public:
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             ErrorCode errorCode,
                             const CefString& errorText,
                             const CefString& failedUrl) OVERRIDE;
    IMPLEMENT_REFCOUNTING(LoadHandler);
};

#endif // LOADHANDLER_H
