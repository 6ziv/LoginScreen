#include "app.h"
#include "cefheaders.h"
#include <Windows.h>
App::App(){
    browser_process_handler=new BrowserProcessHandler();
}
CefRefPtr<CefBrowserProcessHandler> App::GetBrowserProcessHandler(){
    return browser_process_handler;
}
