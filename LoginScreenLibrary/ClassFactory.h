#pragma once
#include <Windows.h>
#include <Shlwapi.h>
#include "RefCounter.h"
#include "DllWrapper.h"
#include "CSampleProvider.h"
#include "guid.h"
class ClassFactory:public IClassFactory
{
private:
	RefCounter cnt;
public:
    inline ClassFactory() :IClassFactory(){}
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void** ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(ClassFactory, IClassFactory),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }
    IFACEMETHODIMP QueryInterfaceAndRelease(__in REFIID riid, __deref_out void** ppv) {
        HRESULT ret = QueryInterface(riid, ppv);
        Release();
        return ret;
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return cnt.AddRef();
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        long cRef = cnt.Release();
        if (cnt.CanUnload())
            delete this;
        return cRef;
    }

    // IClassFactory
    IFACEMETHODIMP CreateInstance(__in IUnknown* pUnkOuter, __in REFIID riid, __deref_out void** ppv)
    {
        HRESULT hr;
        if (!pUnkOuter)
        {
            hr = CSample_CreateInstance(riid, ppv);
        }
        else
        {
            *ppv = NULL;
            hr = CLASS_E_NOAGGREGATION;
        }
        return hr;
    }

    IFACEMETHODIMP LockServer(__in BOOL bLock)
    {
        if (bLock)
            DllWrapper::GetInstance()->AddRef();
        else
            DllWrapper::GetInstance()->Release();
        return S_OK;
    }
    inline static HRESULT CreateInstance(__in REFCLSID rclsid, __in REFIID riid, __deref_out void** ppv){
        *ppv = NULL;
        if (CLSID_CSample != rclsid)
            return CLASS_E_CLASSNOTAVAILABLE;
        ClassFactory* factory = new (std::nothrow)ClassFactory();
        if (factory == nullptr)
            return E_OUTOFMEMORY;
        return factory->QueryInterfaceAndRelease(riid, ppv);
    }
};

