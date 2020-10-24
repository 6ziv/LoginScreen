#include "usernames.h"
std::vector<std::wstring> Usernames::users;
void Usernames::add(std::wstring name){
    users.push_back(name);
}
const std::wstring& Usernames::fetch(size_t id){
    return users[id];
}
size_t Usernames::getsize(){
    return users.size();
}
void Usernames::clear(){
    users.clear();
}
