#ifndef RESOURCEREQUESTHANDLER_H
#define RESOURCEREQUESTHANDLER_H
#include "cefheaders.h"
class CookieAccessFilter;
class ResourceHandler;
class ResourceRequestHandler : public CefResourceRequestHandler
{
public:
    ResourceRequestHandler();
    virtual CefResourceRequestHandler::ReturnValue OnBeforeResourceLoad( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefRequestCallback > callback )OVERRIDE;
    virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, bool& allow_os_execution )OVERRIDE;
    virtual void OnResourceLoadComplete( CefRefPtr<CefBrowser> browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefResponse > response, CefResourceRequestHandler::URLRequestStatus status, int64 received_content_length )OVERRIDE;
    virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefResponse > response, CefString& new_url )OVERRIDE;
    virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefResponse > response )OVERRIDE;
    virtual CefRefPtr<CefCookieAccessFilter> GetCookieAccessFilter( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request )OVERRIDE;
    virtual CefRefPtr<CefResourceHandler> GetResourceHandler( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request )OVERRIDE;
private:
    CefRefPtr<CookieAccessFilter> cookie_access_filter;
    CefRefPtr<ResourceHandler> resource_handler;
    IMPLEMENT_REFCOUNTING(ResourceRequestHandler);
};

#endif // RESOURCEREQUESTHANDLER_H
