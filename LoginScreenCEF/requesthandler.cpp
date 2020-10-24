#include "requesthandler.h"
#include "resourcerequesthandler.h"
RequestHandler::RequestHandler(){
    resource_request_handler=new ResourceRequestHandler;
}
CefRefPtr<CefResourceRequestHandler> RequestHandler::GetResourceRequestHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_navigation, bool is_download, const CefString &request_initiator, bool &disable_default_handling){
    disable_default_handling=true;
    return resource_request_handler;
}
