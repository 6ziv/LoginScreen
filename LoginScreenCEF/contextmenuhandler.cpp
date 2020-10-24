#include "contextmenuhandler.h"
#include "cefheaders.h"
#include "contentloader.h"
void ContextMenuHandler::OnBeforeContextMenu( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model){
    model->Clear();
    model->AddItem(MENU_ID_USER_FIRST,"About this page");
    model->AddItem(MENU_ID_USER_FIRST+1,"Kill (used for emergencies).");
}
bool ContextMenuHandler::OnContextMenuCommand( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags ){
    CEF_REQUIRE_UI_THREAD()
    if(command_id==MENU_ID_USER_FIRST){
        CefSetOSModalLoop(true);
        MessageBoxW(browser->GetHost()->GetWindowHandle(),ContentLoader::getInfo().c_str(),L"msg",MB_OK);
        CefSetOSModalLoop(false);
    }
    if(command_id==MENU_ID_USER_FIRST+1){
        CefQuitMessageLoop();
        CefShutdown();
        ExitProcess(0);
    }
    return true;
}
bool ContextMenuHandler::RunContextMenu( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback ){
    return false;
}
