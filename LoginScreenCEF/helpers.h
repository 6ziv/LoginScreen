#ifndef HELPERS_H
#define HELPERS_H
#include "cefheaders.h"
inline void my_assert(bool val){
    if(!val){
        CefQuitMessageLoop();
    }
}
#endif // HELPERS_H
