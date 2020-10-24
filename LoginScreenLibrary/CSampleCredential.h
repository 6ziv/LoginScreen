//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// CSampleCredential is our implementation of ICredentialProviderCredential.
// ICredentialProviderCredential is what LogonUI uses to let a credential
// provider specify what a user tile looks like and then tell it what the
// user has entered into the tile.  ICredentialProviderCredential is also
// responsible for packaging up the users credentials into a buffer that
// LogonUI then sends on to LSA.

#pragma once
#include <windows.h>
#include <Shlwapi.h>
#include <credentialprovider.h>
#include <strsafe.h>
#include <cstdint>
#include <shlguid.h>
#include <propkey.h>
#include "RefCounter.h"
class CSampleProvider;
class CSampleCredential : public ICredentialProviderCredential, ICredentialProviderCredentialWithFieldOptions
{
    RefCounter cnt;
public:
    // IUnknown
    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return cnt.AddRef();
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        ULONG ret=cnt.Release();
        if (cnt.CanUnload())
            delete this;
        return ret;
    }

    IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(CSampleCredential, ICredentialProviderCredential), // IID_ICredentialProviderCredential
            //QITABENT(CSampleCredential, ICredentialProviderCredential2), // IID_ICredentialProviderCredential2
            QITABENT(CSampleCredential, ICredentialProviderCredentialWithFieldOptions), //IID_ICredentialProviderCredentialWithFieldOptions
            {0},
        };
        return QISearch(this, qit, riid, ppv);
    }
  public:
    // ICredentialProviderCredential
    IFACEMETHODIMP Advise(_In_ ICredentialProviderCredentialEvents *pcpce);
    IFACEMETHODIMP UnAdvise();

    IFACEMETHODIMP SetSelected(_Out_ BOOL *pbAutoLogon) {
        *pbAutoLogon = FALSE;
        return S_OK;
    }
    IFACEMETHODIMP SetDeselected() {
        return S_OK;
    }

    IFACEMETHODIMP GetFieldState(DWORD,
        _Out_ CREDENTIAL_PROVIDER_FIELD_STATE*,
        _Out_ CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE*) {
        return E_INVALIDARG;
    }

    IFACEMETHODIMP GetStringValue(DWORD, _Outptr_result_nullonfailure_ PWSTR *ppwsz) {
        *ppwsz = NULL;
        return E_INVALIDARG;
    }
    IFACEMETHODIMP GetBitmapValue(DWORD, _Outptr_result_nullonfailure_ HBITMAP* phbmp) {
        *phbmp = NULL;
        return E_INVALIDARG;
    }
    IFACEMETHODIMP GetCheckboxValue(DWORD, _Out_ BOOL *, _Outptr_result_nullonfailure_ PWSTR *ppwszLabel) {
        *ppwszLabel = NULL;
        return E_INVALIDARG;
    }
    IFACEMETHODIMP GetComboBoxValueCount(DWORD, _Out_ DWORD *, _Out_ DWORD*) {
        return E_INVALIDARG;
    };
    IFACEMETHODIMP GetComboBoxValueAt(DWORD , DWORD , _Outptr_result_nullonfailure_ PWSTR *ppwszItem) {
        *ppwszItem = NULL;
        return E_INVALIDARG;
    }
    IFACEMETHODIMP GetSubmitButtonValue(DWORD , _Out_ DWORD *) {
        return E_INVALIDARG;
    }

    IFACEMETHODIMP SetStringValue(DWORD, _In_ PCWSTR ) {
        return E_INVALIDARG;
    }
    IFACEMETHODIMP SetCheckboxValue(DWORD, BOOL) {
        return E_INVALIDARG;
    }
    IFACEMETHODIMP SetComboBoxSelectedValue(DWORD, DWORD) {
        return E_INVALIDARG;
    }
    IFACEMETHODIMP CommandLinkClicked(DWORD) {
        return E_INVALIDARG;
    }

    IFACEMETHODIMP GetSerialization(_Out_ CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *pcpgsr,
                                    _Out_ CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs,
                                    _Outptr_result_maybenull_ PWSTR *ppwszOptionalStatusText,
                                    _Out_ CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon);
    IFACEMETHODIMP ReportResult(NTSTATUS ntsStatus,
                                NTSTATUS ntsSubstatus,
                                _Outptr_result_maybenull_ PWSTR *ppwszOptionalStatusText,
                                _Out_ CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon);
    IFACEMETHODIMP GetFieldOptions(DWORD, _Out_ CREDENTIAL_PROVIDER_CREDENTIAL_FIELD_OPTIONS* pcpcfo) 
    {
        *pcpcfo = CPCFO_NONE;
        return E_INVALIDARG;
    }

  public:
    HRESULT Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
                       _In_ ICredentialProviderUser *pcpUser,PWSTR pwd);
    CSampleCredential(CSampleProvider *p);
    bool CheckLogin();
  private:
    PWSTR passwd;
    virtual ~CSampleCredential();
    long                                    _cRef;
    CREDENTIAL_PROVIDER_USAGE_SCENARIO      _cpus;                                          // The usage scenario for which we were enumerated.

    PWSTR                                   _pszUserSid;
    PWSTR                                   _pszQualifiedUserName;                          // The user name that's used to pack the authentication buffer
    ICredentialProviderCredentialEvents2*    _pCredProvCredentialEvents;                    // Used to update fields.
                                                                                            // CredentialEvents2 for Begin and EndFieldUpdates.
    bool                                    _fIsLocalUser;                                  // If the cred prov is assosiating with a local user tile
    
    CSampleProvider* provider;
};
