#ifndef ERRORPAGE_H
#define ERRORPAGE_H
#include <string_view>
#include <algorithm>
class StringStream
{
public:
    ~StringStream(){};
    inline void sethtml(std::string s,std::string m,int e){
        html=s;
        mime=m;
        err=e;
        rewind();
    }
    inline std::string getmime(){return mime;}
    inline int geterr(){return err;}
    inline void rewind(){
        pos=0;
    }
    inline size_t size(){
        return html.size();
    }
    inline size_t getpos(){
        return pos;
    }
    inline void setpos(size_t x){
        pos=x;
    }
    inline size_t read(void* data,size_t cbdata){
        size_t ret=(cbdata<size()-getpos())?cbdata:(size()-getpos());
        memcpy(data,html.data()+pos,ret);
        pos+=ret;
        return ret;
    }
    inline size_t skip(size_t val){
        size_t ret=val;
        if(getpos()+val>size())
            ret=size()-getpos();
        pos=getpos()+ret;
        return ret;
    }
private:
    int err;
    std::string mime;
    std::string html;
    size_t pos=0;
};

#endif // ERRORPAGE_H
