#include "avatarloader.h"
#include "mimeguess.h"
#include <Windows.h>
#include <sddl.h>
FILE* AvatarLoader::OpenFile(wchar_t *path){
    FILE* f=_wfopen(path,L"rb");
    return f;
}
bool AvatarLoader::open(const std::wstring uname){
    SID_NAME_USE nuse;
    DWORD sid_size=0;
    DWORD name_size=0;
    LookupAccountNameW(L".",uname.c_str(),NULL,&sid_size,NULL,&name_size,&nuse);
    PSID psid=reinterpret_cast<PSID>(malloc(sid_size));
    PWSTR name=reinterpret_cast<PWSTR>(malloc(name_size*sizeof(WCHAR)));
    if(psid==nullptr || name==nullptr)return false;
    BOOL ok=LookupAccountNameW(L".",uname.c_str(),psid,&sid_size,name,&name_size,&nuse);
    if((!ok) || nuse!=SidTypeUser)
        return false;
    LPWSTR strsid;
    ok=ConvertSidToStringSidW(psid,&strsid);
    free(psid);
    free(name);
    std::wstring regpath;
    if(ok){
        regpath=std::wstring()+L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AccountPicture\\Users\\"+strsid;
        LocalFree(strsid);
    }
    else
        return false;
    HKEY hKey;
    LSTATUS ret=RegOpenKeyExW(HKEY_LOCAL_MACHINE,regpath.c_str(),NULL,KEY_READ,&hKey);
    if(ret!=ERROR_SUCCESS)
        return false;
    current_res=0;
    hwnd=nullptr;
    ext=L"";
    for(DWORD index=0;true;index++){
        WCHAR valname[32768];
        DWORD cbvalname=32767;
        DWORD valtype;
        DWORD datasize=0;
        LSTATUS ret=RegEnumValueW(hKey,index,valname,&cbvalname,NULL,&valtype,NULL,&datasize);
        datasize+=sizeof(WCHAR);
        if(ret==ERROR_NO_MORE_ITEMS)
            break;
        if(valtype!=REG_SZ || ret!=ERROR_SUCCESS || cbvalname<6){
            continue;
        }
        bool match=true;
        for(int i=0;i<5;i++){
            if(valname[i]!=L"Image"[i])
                match=false;
        }
        if(!match){
            continue;
        }
        long long res=_wtoll(valname+5);

        if(res<=0)continue;
        if(static_cast<size_t>(res)<=current_res)continue;
        PWSTR data=reinterpret_cast<PWSTR>(malloc(datasize));
        if(data==nullptr){
            continue;
        }
        cbvalname=32767;
        memset(data,0,datasize+sizeof(WCHAR));

        ret=RegEnumValueW(hKey,index,valname,&cbvalname,NULL,&valtype,reinterpret_cast<LPBYTE>(data),&datasize);
        if(valtype!=REG_SZ || ret!=ERROR_SUCCESS){
            free(data);
            continue;
        }
        FILE* tmphwnd=OpenFile(data);

        std::wstring path=std::wstring(data);
        auto pos=path.find_last_of(L"./");
        if(pos!=path.npos && path[pos]=='.')
            ext=path.substr(pos+1);
        else
            ext=L"";
        free(data);
        if(tmphwnd==nullptr){
            continue;
        }
        if(hwnd!=nullptr)
            fclose(hwnd);
        hwnd=tmphwnd;
        current_res=res;
    }
    return hwnd!=nullptr;
}
size_t AvatarLoader::read(void* data,size_t size){
    return fread(data,1,size,hwnd);
}
bool AvatarLoader::seek(int64_t offset,int pos){
    fseek(hwnd,offset,pos);
    return true;
}
int64_t AvatarLoader::tell(){
    return ftell(hwnd);
}
void AvatarLoader::close(){
    fclose(hwnd);
}
int64_t AvatarLoader::getSize(){
    struct _stat64 s;
    _fstati64(_fileno(hwnd),&s);
    return s.st_size;
}
std::string AvatarLoader::getComment(){
    std::string str(ext.begin(),ext.end());
    return std::string(MIMEGuess::guess(str));
}
