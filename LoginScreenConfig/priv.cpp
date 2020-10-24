#include "priv.h"

#include <Windows.h>
#include <WinBase.h>
#include <Shlwapi.h>
#include <QApplication>

bool Priv::IsAdmin(){
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                  DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
                                  &AdministratorsGroup))
        {
            return false;
        }
    BOOL b = FALSE;
    if (!CheckTokenMembership(NULL, AdministratorsGroup, &b))
        {
            b = FALSE;
        }
    FreeSid(AdministratorsGroup);
    return b == TRUE;
}
bool Priv::RunAsAdmin(QString arg){
    wchar_t *SelfPath;
    DWORD PathLen;
    PathLen=MAX_PATH;
    while(PathLen<65536){
        SetLastError(0);
        SelfPath=reinterpret_cast<wchar_t*>(malloc(PathLen*sizeof(wchar_t)));
        DWORD ret=GetModuleFileNameW(NULL,SelfPath,PathLen);
        if(ret==0){
            return false;
        }
        if(ret==PathLen && GetLastError()==ERROR_INSUFFICIENT_BUFFER){
            free(SelfPath);
            PathLen+=MAX_PATH;
            continue;
        }
        break;
    }
    //wchar_t *Cmdline
    INT NewExe=(INT)ShellExecuteW(NULL,L"runas",SelfPath,arg.toStdWString().c_str(),NULL,SW_SHOW);
    if(NewExe<=32){
        return false;
    }
    else{
        ExitProcess(0);
    }
}
