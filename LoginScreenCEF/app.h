#ifndef APP_H
#define APP_H
#include "cefheaders.h"
#include "browserprocesshandler.h"
class App : public CefApp{
public:
    App();
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()OVERRIDE;
private:
    CefRefPtr<BrowserProcessHandler> browser_process_handler;
    IMPLEMENT_REFCOUNTING(App);
};

#endif // APP_H
