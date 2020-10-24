#include <iostream>
#include "conf.h"
#include "app.h"
#include "cefheaders.h"
#include "helpers.h"
#include <map>
#include <iostream>
#include "avatarloader.h"
#include "Login.h"
#include <imm.h>
using namespace std;
#pragma comment(lib,"IMM32.lib")
int main(int argc,char** argv)
{
    try{
    ImmDisableIME(-1);
    ShowWindow(GetConsoleWindow(),SW_HIDE);
    CefEnableHighDPISupport();
    //CefScopedSandboxInfo scoped_sandbox;
    //void *sandbox_info = scoped_sandbox.sandbox_info();
    HINSTANCE instance=GetModuleHandleA(nullptr);
    my_assert(instance!=nullptr);
    CefMainArgs args(instance);
    CefRefPtr<App> app=new App;
    int ret=CefExecuteProcess(args,app,nullptr);
    if(ret>=0)return ret;
    if(ret==-1){
        if(!Login::getInstance()->Init(argc,argv))exit(1);
    }
    CefSettings settings;
    Conf::InitSettings(settings);

    CefInitialize(args,settings,app,nullptr);
    CefRunMessageLoop();
    CefShutdown();
    return 0;
    }catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
}

