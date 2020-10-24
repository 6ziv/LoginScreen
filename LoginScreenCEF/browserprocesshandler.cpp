#include "browserprocesshandler.h"
#include "client.h"
#include "conf.h"
#include "cefheaders.h"
void BrowserProcessHandler::OnContextInitialized(){
    CEF_REQUIRE_UI_THREAD()

    CefRefPtr<Client> client=new Client;
    CefBrowserSettings browser_settings;
    Conf::InitBrowserSettings(browser_settings);
    std::string url = "http://localhost_lockscreen/index.html";
    CefWindowInfo window_info;
    window_info.SetAsPopup(NULL, "cefsimple");
    CefBrowserHost::CreateBrowser(window_info, client, url, browser_settings,nullptr, nullptr);
}
