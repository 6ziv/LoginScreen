#include "client.h"
#include "displayhandler.h"
#include "lifespanhandler.h"
#include "loadhandler.h"
#include "contextmenuhandler.h"
#include "requesthandler.h"
Client::Client(){
    display_handler=new DisplayHandler;
    lifespan_handler=new LifespanHandler;
    load_handler=new LoadHandler;
    context_menu_handler=new ContextMenuHandler;
    request_handler=new RequestHandler;
}
CefRefPtr<CefDisplayHandler> Client::GetDisplayHandler(){
    return display_handler;
}
CefRefPtr<CefLifeSpanHandler> Client::GetLifeSpanHandler(){
    return lifespan_handler;
}
CefRefPtr<CefLoadHandler> Client::GetLoadHandler(){
    return load_handler;
}
CefRefPtr<CefContextMenuHandler> Client::GetContextMenuHandler(){
    return context_menu_handler;
}
CefRefPtr<CefRequestHandler> Client::GetRequestHandler(){
    return request_handler;
}

