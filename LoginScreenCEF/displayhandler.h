#ifndef DISPLAYHANDLER_H
#define DISPLAYHANDLER_H

#include "cefheaders.h"

#include <list>

class DisplayHandler:public CefDisplayHandler{
public:
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,const CefString& title) OVERRIDE;
    IMPLEMENT_REFCOUNTING(DisplayHandler);
};
#endif
