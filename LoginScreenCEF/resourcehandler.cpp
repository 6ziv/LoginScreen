#include "resourcehandler.h"
#include "contentloader.h"
#include "stringstream.h"
#include <iostream>
#include "Login.h"
#include <Shlwapi.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/encodings.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <utf8cpp/utf8.h>
#include "conf.h"
ResourceHandler::ResourceHandler()
{
    f=NULL;
    auto res=Conf::getResourceFile();
    if(res.empty())ExitProcess(0);
    content_loader=std::make_unique<ContentLoader>(res);
    string_stream=std::make_unique<StringStream>();
    avatar_loader=std::make_unique<AvatarLoader>();
}
void ResourceHandler::Cancel(){
    if(state==CSTATE_SUCCESS)
        content_loader->close();
    if(state==CSTATE_AVATAR)
        avatar_loader->close();
    lock=false;
}
void ResourceHandler::GetResponseHeaders( CefRefPtr< CefResponse > response, int64& response_length, CefString& redirectUrl ){
    switch (state) {
    case CSTATE_SUCCESS:
    {
        size=content_loader->getSize();
        response_length=size;
        response->SetStatus(200);
        std::string mime=content_loader->getComment();
        if(!mime.empty()){
            response->SetMimeType(mime.c_str());
        }else{
            response->SetMimeType(MIMEGuess::guess(ext).data());
        }
        break;
    }
    case CSTATE_ERROR_OCCURRED:
    {
        response->SetStatus(302);
        redirectUrl="http://localhost_lockscreen/internal_error.html";
        break;
    }
    case CSTATE_STRING_STREAM:
    {
        response_length=string_stream->size();
        response->SetStatus(string_stream->geterr());
        response->SetMimeType(string_stream->getmime());
        break;
    }
    case CSTATE_AVATAR:
    {
        size=avatar_loader->getSize();
        response_length=size;
        response->SetStatus(200);
        response->SetMimeType(avatar_loader->getComment());
        break;
    }
    }
}
bool ResourceHandler::Open( CefRefPtr< CefRequest > request, bool& handle_request, CefRefPtr< CefCallback > callback ){
    locklock.lock();
    while(lock)Sleep(1);
    lock=true;
    locklock.unlock();
    const std::string prefix="http://localhost_lockscreen/";
    std::string url=request->GetURL().ToString();
    if(url.substr(0,prefix.length()).compare(prefix)!=0){
        lock=false;
        return false;
    }
    std::string path=url.substr(prefix.length());
    state=CSTATE_SUCCESS;
    if(path.compare("internal_error.html")==0){
        state=CSTATE_STRING_STREAM;
        string_stream->sethtml("File Not Found!","text/html",404);
        handle_request=true;
        ext="html";
        return true;
    }
    if(path.compare("getusers.do")==0){
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer,rapidjson::UTF16LE<>,rapidjson::ASCII<>> writer(buffer);
        writer.StartArray();
        for(auto& usr:Login::getInstance()->users){
            writer.String(usr.c_str(),usr.length());
         //   auto u=cJSON_Crea
        }
        writer.EndArray();
        writer.Flush();
        //Login::getInstance()->users
        state=CSTATE_STRING_STREAM;
        string_stream->sethtml(buffer.GetString(),"text/json",200);
        handle_request=true;
        ext="json";
        return true;
    }
    if(path.compare("login.do")==0){
        CefPostData::ElementVector elements;
        request->GetPostData()->GetElements(elements);
        auto cnt=elements.at(0)->GetBytesCount();
        char* tmp=new char[cnt+1];
        elements.at(0)->GetBytes(cnt,tmp);
        tmp[cnt]='\0';
        std::wstring ws;
        utf8::utf8to16(tmp,tmp+cnt,std::back_inserter(ws));
        delete[] tmp;
        wchar_t* wtmp=new wchar_t[ws.length()+1];
        wcscpy(wtmp,ws.c_str());
        DWORD ret;
        UrlUnescapeW(wtmp,NULL,&ret,URL_UNESCAPE_AS_UTF8|URL_UNESCAPE_INPLACE);
        ret=wcslen(wtmp);
        //MessageBoxW(0,wtmp,L"post",MB_OK);
        unsigned long long ulluid;
        std::wstring pass;
        size_t pos=0;
        while(pos<ret){
            if(wcsncmp(wtmp+pos,L"uid=",4)==0){
                size_t epos=pos+4;
                while(epos<ret && wtmp[epos]!=L'&')
                    epos++;
                std::wstring uid(wtmp+pos+4,wtmp+epos);
                ulluid=wcstoull(uid.c_str(),nullptr,10);
                pos=epos;
                if(pos<ret)
                    pos++;
            }else
            if(wcsncmp(wtmp+pos,L"passwd=",7)==0){
                size_t epos=pos+7;
                while(epos<ret && wtmp[epos]!=L'&')
                    epos++;
                pass=std::wstring(wtmp+pos+7,wtmp+epos);
                pos=epos;
                if(pos<ret)
                    pos++;
            }else{
                while(pos<ret && wtmp[pos]!=L'&')
                    pos++;
                if(pos<ret)
                    pos++;
            }
        }
        Login::getInstance()->doLogin(ulluid,pass.c_str(),pass.length());
        state=CSTATE_STRING_STREAM;
        string_stream->sethtml("{ret:1}","text/json",200);
        handle_request=true;
        ext="json";

        return true;
    }
    const std::wstring prefix1=L"http://localhost_lockscreen/avatar/";
    std::wstring wurl=request->GetURL().ToWString();
    if(wurl.substr(0,prefix1.length()).compare(prefix1)==0){
        std::wstring usrname=wurl.substr(prefix1.length());
        handle_request=true;
        if(!avatar_loader->open(usrname))
            state=CSTATE_ERROR_OCCURRED;
        else
            state=CSTATE_AVATAR;
        return true;
    }
    bool loaded=content_loader->open(path.c_str());
    handle_request=true;
    auto pos=path.find_last_of("./");
    ext="";
    if(pos!=path.npos && path[pos]=='.')
        ext=path.substr(pos+1);
    if(!loaded)state=CSTATE_ERROR_OCCURRED;else state=CSTATE_SUCCESS;
    return true;
}
bool ResourceHandler::ProcessRequest( CefRefPtr< CefRequest > request, CefRefPtr< CefCallback > callback ){
    //deprecated
    return false;
}
bool ResourceHandler::Read( void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr< CefResourceReadCallback > callback )
{

    if(bytes_to_read<0)return false;
    size_t btr=static_cast<size_t>(bytes_to_read);
    switch(state){
    case CSTATE_SUCCESS:
        bytes_read=static_cast<int>(content_loader->read(data_out,btr));
        break;
    case CSTATE_STRING_STREAM:
        bytes_read=static_cast<int>(string_stream->read(data_out,btr));
        break;
    case CSTATE_ERROR_OCCURRED:
        bytes_read=0;
        break;
    case CSTATE_AVATAR:
        bytes_read=static_cast<int>(avatar_loader->read(data_out,btr));
        break;
    }
    if(bytes_read==0)return false;else return true;
}
bool ResourceHandler::ReadResponse( void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr< CefCallback > callback ){
    //deprecated
    return false;
}
bool ResourceHandler::Skip( int64 bytes_to_skip, int64& bytes_skipped, CefRefPtr< CefResourceSkipCallback > callback ){
    switch(state){
    case CSTATE_SUCCESS:
    {
        int64_t cur=content_loader->tell();
        if(static_cast<size_t>(cur+bytes_to_skip)>size)bytes_skipped=size-cur;else bytes_skipped=bytes_to_skip;
        content_loader->seek(bytes_skipped,SEEK_CUR);
        return true;
    }
    case CSTATE_ERROR_OCCURRED:
    {
        bytes_skipped=-2;
        return false;
    }
    case CSTATE_STRING_STREAM:
    {
        bytes_skipped=string_stream->skip(bytes_to_skip);
        return true;
    }
    case CSTATE_AVATAR:
    {
        int64_t cur=avatar_loader->tell();
        if(static_cast<size_t>(cur+bytes_to_skip)>size)bytes_skipped=size-cur;else bytes_skipped=bytes_to_skip;
        content_loader->seek(bytes_skipped,SEEK_CUR);
        return true;
    }
    }
    return false;
}
