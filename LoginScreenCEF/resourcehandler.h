#ifndef RESOURCEHANDLER_H
#define RESOURCEHANDLER_H
#include "cefheaders.h"
#include "mimeguess.h"
#include "avatarloader.h"
#include <atomic>
#include <mutex>
class ContentLoader;
class StringStream;
class ResourceHandler : public CefResourceHandler
{
public:
    ResourceHandler();
    virtual void Cancel()OVERRIDE;
    virtual void GetResponseHeaders( CefRefPtr< CefResponse > response, int64& response_length, CefString& redirectUrl )OVERRIDE;
    virtual bool Open( CefRefPtr< CefRequest > request, bool& handle_request, CefRefPtr< CefCallback > callback )OVERRIDE;
    virtual bool ProcessRequest( CefRefPtr< CefRequest > request, CefRefPtr< CefCallback > callback )OVERRIDE;
    virtual bool Read( void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr< CefResourceReadCallback > callback )OVERRIDE;
    virtual bool ReadResponse( void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr< CefCallback > callback )OVERRIDE;
    virtual bool Skip( int64 bytes_to_skip, int64& bytes_skipped, CefRefPtr< CefResourceSkipCallback > callback )OVERRIDE;
private:
    std::atomic_bool lock=false;
    std::mutex locklock;
    FILE* f;
    size_t size;
    std::unique_ptr<ContentLoader> content_loader;
    std::unique_ptr<AvatarLoader> avatar_loader;
    std::unique_ptr<StringStream> string_stream;
    std::string ext;
    enum STATE{
        CSTATE_SUCCESS,
        CSTATE_AVATAR,
        CSTATE_ERROR_OCCURRED,
        CSTATE_STRING_STREAM
    }state=CSTATE_SUCCESS;
    IMPLEMENT_REFCOUNTING(ResourceHandler);
};

#endif // RESOURCEHANDLER_H
