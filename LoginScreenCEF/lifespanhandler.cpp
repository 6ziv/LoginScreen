#include "lifespanhandler.h"
#include "cefheaders.h"
void LifespanHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD()
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    int horizontal = desktop.right;
    int vertical = desktop.bottom;
    HWND browserhwnd = static_cast<HWND>(browser->GetHost()->GetWindowHandle());
    SetWindowLongPtr(browserhwnd, GWL_STYLE, WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
    MoveWindow(browserhwnd, 0, 0, horizontal, vertical, TRUE);

    SetWindowPos(browserhwnd,HWND_TOPMOST,0,0,horizontal, vertical,SWP_SHOWWINDOW);
}

bool LifespanHandler::DoClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD()
    return false;
}

void LifespanHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD()
    CefQuitMessageLoop();
}
