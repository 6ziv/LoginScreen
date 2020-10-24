#include "loadhandler.h"
#include "cefheaders.h"
void LoadHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             ErrorCode errorCode,
                             const CefString& errorText,
                              const CefString& failedUrl)
{
    CEF_REQUIRE_UI_THREAD();
    if (errorCode == ERR_ABORTED)
        return;
}
