#ifndef COOKIEACCESSFILTER_H
#define COOKIEACCESSFILTER_H

#include "cefheaders.h"
class CookieAccessFilter : public CefCookieAccessFilter
{
public:
    virtual bool CanSaveCookie( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefResponse > response, const CefCookie& cookie )OVERRIDE;
    virtual bool CanSendCookie( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, const CefCookie& cookie )OVERRIDE;
    IMPLEMENT_REFCOUNTING(CookieAccessFilter);
};

#endif // COOKIEACCESSFILTER_H
