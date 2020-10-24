#ifndef CLIENT_H
#define CLIENT_H
#include "cefheaders.h"
class DisplayHandler;
class LifespanHandler;
class LoadHandler;
class ContextMenuHandler;
class RequestHandler;
class Client : public CefClient
{
public:
    Client();
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE;
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE;
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE;
private:
    CefRefPtr<DisplayHandler> display_handler;
    CefRefPtr<LifespanHandler> lifespan_handler;
    CefRefPtr<LoadHandler> load_handler;
    CefRefPtr<ContextMenuHandler> context_menu_handler;
    CefRefPtr<RequestHandler> request_handler;
    IMPLEMENT_REFCOUNTING(Client);

};

#endif // CLIENT_H
