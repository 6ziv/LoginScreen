#include "resourcerequesthandler.h"
#include "cookieaccessfilter.h"
#include "resourcehandler.h"
ResourceRequestHandler::ResourceRequestHandler()
{
    cookie_access_filter=new CookieAccessFilter;
    resource_handler=new ResourceHandler;
}
CefRefPtr<CefCookieAccessFilter> ResourceRequestHandler::GetCookieAccessFilter( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request ){
    return cookie_access_filter;
}
CefRefPtr<CefResourceHandler> ResourceRequestHandler::GetResourceHandler( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request ){
    return resource_handler;
}
CefResourceRequestHandler::ReturnValue ResourceRequestHandler::OnBeforeResourceLoad( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefRequestCallback > callback ){
    std::wstring url=request->GetURL().ToWString();
    const std::wstring prefix=L"http://localhost_lockscreen/";
    if(url.substr(0,prefix.length()).compare(prefix)!=0)
        return RV_CANCEL;
    else
        return RV_CONTINUE;
}
void ResourceRequestHandler::OnProtocolExecution( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, bool& allow_os_execution ){
    allow_os_execution=false;
}
void ResourceRequestHandler::OnResourceLoadComplete( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefResponse > response, CefResourceRequestHandler::URLRequestStatus status, int64 received_content_length ){
}
void ResourceRequestHandler::OnResourceRedirect( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefResponse > response, CefString& new_url ){
}
bool ResourceRequestHandler::OnResourceResponse( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefRequest > request, CefRefPtr< CefResponse > response ){
    return false;
}
