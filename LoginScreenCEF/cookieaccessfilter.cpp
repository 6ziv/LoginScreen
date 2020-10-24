#include "cookieaccessfilter.h"

bool CookieAccessFilter::CanSaveCookie( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefResponse > response, const CefCookie& cookie ){
    return false;
}
bool CookieAccessFilter::CanSendCookie( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, const CefCookie& cookie ){
    return false;
}
