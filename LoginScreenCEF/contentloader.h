#ifndef CONTENTLOADER_H
#define CONTENTLOADER_H
#include <string>
#include <zip.h>
class ContentLoader
{
public:
    ContentLoader(const std::wstring filename);
    bool open(const char* fname);
    size_t read(void* data,size_t size);
    bool seek(int64_t offset,int pos);
    int64_t tell();
    void close();
    const std::string& getComment();
    int64_t getSize();
    static inline const std::wstring& getInfo(){return archive_info;}
private:
    zip_file_t *file=nullptr;
    zip_stat_t stat;
    zip_t *zip;
    std::string comment;
    int64_t size;
    static inline std::wstring archive_info=L"";
};

#endif // CONTENTLOADER_H
