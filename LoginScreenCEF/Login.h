#pragma once
#include <Windows.h>
#include <string>
#include <vector>
class Login
{
private:
    HANDLE sigDone = INVALID_HANDLE_VALUE;
    HANDLE sigReady = INVALID_HANDLE_VALUE;
    HANDLE hWrite;
    inline Login(){}
    inline bool GetContent(HANDLE hRead) {
        DWORD rSize;

        if ((!ReadFile(hRead, &sigDone, sizeof(HANDLE), &rSize, NULL))||(rSize != sizeof(HANDLE)))return false;
        if ((!ReadFile(hRead, &sigReady, sizeof(HANDLE), &rSize, NULL)) || (rSize != sizeof(HANDLE)))return false;

        uint64_t usrSize;

        if ((!ReadFile(hRead, &usrSize, sizeof(uint64_t), &rSize, NULL)) || (rSize != sizeof(uint64_t)))return false;
        wchar_t* str = new (std::nothrow)wchar_t[usrSize+1];
        if (str == nullptr)return false;
        str[usrSize] = L'\0';

        if ((!ReadFile(hRead, str, usrSize * sizeof(wchar_t), &rSize, NULL)) || (rSize != usrSize * sizeof(wchar_t)))return false;
        size_t pos = 0;
        while (pos < usrSize) {
            std::wstring usr = std::wstring(str + pos);
            pos += usr.length() + 1;
            users.push_back(usr);
        }
        return true;
    }
    inline static Login* instance=nullptr;
public:
    inline static Login* getInstance() {
        if (!instance)instance = new Login();
        return instance;
    }
    inline bool Init(int argc, wchar_t** argv) {
        if (argc != 3)
            return false;
        HANDLE rHandle = reinterpret_cast<HANDLE>(std::wcstoull(argv[1], nullptr, 10));
        HANDLE wHandle = reinterpret_cast<HANDLE>(std::wcstoull(argv[2], nullptr, 10));
        if (rHandle == NULL || rHandle == INVALID_HANDLE_VALUE || wHandle == NULL || wHandle == INVALID_HANDLE_VALUE)
            return false;
        if (!GetContent(rHandle))return false;
        hWrite = wHandle;
        return true;
    }
    inline bool Init(int argc, char** argv) {
        if (argc != 3)return false;
        HANDLE rHandle = reinterpret_cast<HANDLE>(std::strtoull(argv[1], nullptr, 10));
        HANDLE wHandle = reinterpret_cast<HANDLE>(std::strtoull(argv[2], nullptr, 10));
        if (rHandle == NULL || rHandle == INVALID_HANDLE_VALUE || wHandle == NULL || wHandle == INVALID_HANDLE_VALUE)
            return false;
        if (!GetContent(rHandle))return false;
        hWrite = wHandle;
        return true;
    }
    inline bool doLogin(DWORD uid,const wchar_t* passwd,size_t pLen) {
        if (sigDone == INVALID_HANDLE_VALUE)return false;
        if (sigReady == INVALID_HANDLE_VALUE)return false;
        //WaitForSingleObject(sigReady,INFINITE);
        SetEvent(sigDone);
        DWORD userid = uid;
        DWORD rSize;
        uint64_t pwd_length = pLen;
        if ((!WriteFile(hWrite, &userid, sizeof(DWORD), &rSize, NULL)) || (rSize != sizeof(DWORD)))return false;
        if ((!WriteFile(hWrite, &pwd_length, sizeof(uint64_t), &rSize, NULL)) || (rSize != sizeof(uint64_t)))return false;
        if ((!WriteFile(hWrite, passwd, (pLen + 1) * sizeof(wchar_t), &rSize, NULL)) || (rSize != (pLen + 1) * sizeof(wchar_t)))return false;
        WaitForSingleObject(sigReady,INFINITE);
        return true;
    }
    std::vector<std::wstring> users;
};

