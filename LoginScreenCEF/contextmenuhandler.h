#ifndef CONTEXTMENUHANDLER_H
#define CONTEXTMENUHANDLER_H

#include "cefheaders.h"
class ContextMenuHandler : public CefContextMenuHandler
{
public:
    virtual void OnBeforeContextMenu( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)OVERRIDE;
    virtual bool OnContextMenuCommand( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags )OVERRIDE;
    //virtual void OnContextMenuDismissed( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame )OVERRIDE;
    virtual bool RunContextMenu( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback )OVERRIDE;
    IMPLEMENT_REFCOUNTING(ContextMenuHandler);
};

#endif // CONTEXTMENUHANDLER_H
