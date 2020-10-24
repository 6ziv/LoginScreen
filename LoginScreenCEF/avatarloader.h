#ifndef AVATARLOADER_H
#define AVATARLOADER_H
#include <string>
class AvatarLoader
{
public:
    bool open(const std::wstring uname);
    size_t read(void* data,size_t size);
    bool seek(int64_t offset,int pos);
    int64_t tell();
    void close();
    int64_t getSize();
    std::string getComment();
private:
    std::wstring ext;
    FILE* OpenFile(wchar_t *path);
    size_t current_res;
    FILE* hwnd;
};

#endif // AVATARLOADER_H
