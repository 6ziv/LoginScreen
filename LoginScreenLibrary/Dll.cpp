//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Standard dll required functions and class factory implementation.

#include <windows.h>
#include <unknwn.h>
#include "helpers.h"
#include "ClassFactory.h"
#include "DllWrapper.h"

STDAPI DllCanUnloadNow()
{
    return DllWrapper::GetInstance()->CanUnload() ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(__in REFCLSID rclsid, __in REFIID riid, __deref_out void** ppv)
{
    return DllWrapper::GetInstance()->GetClassObject(rclsid, riid, ppv);
}

STDAPI_(BOOL) DllMain(__in HINSTANCE hinstDll, __in DWORD dwReason, __in void *)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDll);
        DllWrapper::GetInstance()->hinst=hinstDll;
        break;
    case DLL_PROCESS_DETACH:
        DllWrapper::FreeInstance();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

