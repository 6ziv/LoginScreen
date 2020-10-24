#ifndef USERNAMES_H
#define USERNAMES_H
#include <string>
#include <vector>

class Usernames
{
public:
    static void add(std::wstring);
    static const std::wstring& fetch(size_t);
    static size_t getsize();
    static void clear();
private:
    static std::vector<std::wstring> users;
};

#endif // USERNAMES_H
