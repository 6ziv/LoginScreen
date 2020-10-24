#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H
#include "cefheaders.h"
class ResourceRequestHandler;
class RequestHandler : public CefRequestHandler
{
public:
    RequestHandler();
    virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool is_navigation,
        bool is_download,
        const CefString& request_initiator,
        bool& disable_default_handling)OVERRIDE;
private:
    CefRefPtr<ResourceRequestHandler> resource_request_handler;

    IMPLEMENT_REFCOUNTING(RequestHandler);
};

#endif // REQUESTHANDLER_H
