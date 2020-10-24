#include "displayhandler.h"
#include "cefheaders.h"
void DisplayHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title) {
    CEF_REQUIRE_UI_THREAD()
    CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
    SetWindowTextW(hwnd, std::wstring(title).c_str());
}

