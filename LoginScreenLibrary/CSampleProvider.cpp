#define _CRT_SECURE_NO_WARNINGS
#include <initguid.h>
#include "CSampleProvider.h"
#include "CSampleCredential.h"
#include "guid.h"
#include <thread>
#include "DllWrapper.h"
CSampleProvider::CSampleProvider() :
	_cRef(1),
	_pCredProviderUserArray(nullptr),
	do_login(false),
	spcpe(nullptr),
	Credential(nullptr)
{
	DllWrapper::GetInstance()->AddRef();
}
CSampleProvider::~CSampleProvider()
{
	KillWorker();
	if (Credential != nullptr)
		Credential->Release();
	Credential = nullptr;
	if (_pCredProviderUserArray != nullptr)
	{
		_pCredProviderUserArray->Release();
		_pCredProviderUserArray = nullptr;
	}
	DllWrapper::GetInstance()->Release();
}

// SetUsageScenario is the provider's cue that it's going to be asked for tiles
// in a subsequent call.
HRESULT CSampleProvider::SetUsageScenario(
	CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
	DWORD /*dwFlags*/)
{
	DWORD cbSize = 0;
	HRESULT hr;
	if (ERROR_SUCCESS != RegGetValueW(HKEY_LOCAL_MACHINE, L"Software\\LoginScreen", L"Application", RRF_RT_REG_EXPAND_SZ | RRF_RT_REG_SZ, NULL, NULL, &cbSize)) {
        isOk = false;
		return E_NOTIMPL;
	}
	wchar_t* buffer = reinterpret_cast<wchar_t*>(malloc(cbSize));
	if (!buffer) {
        isOk = false;
		return E_NOTIMPL;
	}
	if (ERROR_SUCCESS != RegGetValueW(HKEY_LOCAL_MACHINE, L"Software\\LoginScreen", L"Application", RRF_RT_REG_EXPAND_SZ | RRF_RT_REG_SZ, NULL, buffer, &cbSize)) {
        isOk = false;
		return E_NOTIMPL;
	}
	this->exePath = std::wstring(buffer);
	free(buffer);

	// Decide which scenarios to support here. Returning E_NOTIMPL simply tells the caller
	// that we're not designed for that scenario.
	switch (cpus)
	{
	case CPUS_LOGON:
	case CPUS_UNLOCK_WORKSTATION:
		// The reason why we need _fRecreateEnumeratedCredentials is because ICredentialProviderSetUserArray::SetUserArray() is called after ICredentialProvider::SetUsageScenario(),
		// while we need the ICredentialProviderUserArray during enumeration in ICredentialProvider::GetCredentialCount()
		_cpus = cpus;
		_fRecreateEnumeratedCredentials = true;
		hr = S_OK;
		do_login = false;
		break;

	case CPUS_CHANGE_PASSWORD:
	case CPUS_CREDUI:
		hr = E_NOTIMPL;
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}

	return hr;
}
HRESULT CSampleProvider::SetSerialization(
	_In_ CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION const* /*pcpcs*/)
{
	return E_NOTIMPL;
}

// Called by LogonUI to give you a callback.  Providers often use the callback if they
// some event would cause them to need to change the set of tiles that they enumerated.
HRESULT CSampleProvider::Advise(
	_In_ ICredentialProviderEvents* pcpe,
	_In_ UINT_PTR upAdviseContext)
{
	pcpe->AddRef();
	spcpe = pcpe;
	upAdviseCtx = upAdviseContext;
	return S_OK;
}

// Called by LogonUI when the ICredentialProviderEvents callback is no longer valid.
HRESULT CSampleProvider::UnAdvise()
{
	if (spcpe)
		spcpe->Release();
	KillWorker();
	return S_OK;
}

// Called by LogonUI to determine the number of fields in your tiles.  This
// does mean that all your tiles must have the same number of fields.
// This number must include both visible and invisible fields. If you want a tile
// to have different fields from the other tiles you enumerate for a given usage
// scenario you must include them all in this count and then hide/show them as desired
// using the field descriptors.
HRESULT CSampleProvider::GetFieldDescriptorCount(
	_Out_ DWORD* pdwCount)
{
	*pdwCount = 0;
	return S_OK;
}

// Gets the field descriptor for a particular field.
HRESULT CSampleProvider::GetFieldDescriptorAt(
	DWORD,
	_Outptr_result_nullonfailure_ CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR** ppcpfd)
{
	*ppcpfd = nullptr;
	return E_INVALIDARG;
}

// Sets pdwCount to the number of tiles that we wish to show at this time.
// Sets pdwDefault to the index of the tile which should be used as the default.
// The default tile is the tile which will be shown in the zoomed view by default. If
// more than one provider specifies a default the last used cred prov gets to pick
// the default. If *pbAutoLogonWithDefault is TRUE, LogonUI will immediately call
// GetSerialization on the credential you've specified as the default and will submit
// that credential for authentication without showing any further UI.
HRESULT CSampleProvider::GetCredentialCount(
	_Out_ DWORD* pdwCount,
	_Out_ DWORD* pdwDefault,
	_Out_ BOOL* pbAutoLogonWithDefault)
{
	selected_user = 0;

	if (do_login) {
		*pdwCount = 1;
		*pdwDefault = 0;
		*pbAutoLogonWithDefault = TRUE;
		return S_OK;
	}
	*pdwCount = 0;
	*pdwDefault = CREDENTIAL_PROVIDER_NO_DEFAULT;
	*pbAutoLogonWithDefault = FALSE;

	return S_OK;
}

// Returns the credential at the index specified by dwIndex. This function is called by logonUI to enumerate
// the tiles.
HRESULT CSampleProvider::GetCredentialAt(
	DWORD dwIndex,
	_Outptr_result_nullonfailure_ ICredentialProviderCredential** ppcpc)
{
	HRESULT hr = E_INVALIDARG;
	*ppcpc = nullptr;
	if ((dwIndex < 1) && ppcpc)
	{
		hr = Credential->QueryInterface(IID_PPV_ARGS(ppcpc));
	}
	return hr;
}

// This function will be called by LogonUI after SetUsageScenario succeeds.
// Sets the User Array with the list of users to be enumerated on the logon screen.
HRESULT CSampleProvider::SetUserArray(_In_ ICredentialProviderUserArray* usrs)
{
	if (_pCredProviderUserArray)
	{
		_pCredProviderUserArray->Release();
	}
	_pCredProviderUserArray = usrs;
	_pCredProviderUserArray->AddRef();
	DWORD cnt;
	_pCredProviderUserArray->GetCount(&cnt);
	for (DWORD i = 0; i < cnt; i++) {
		ICredentialProviderUser* usr;
		_pCredProviderUserArray->GetAt(i, &usr);
		LPWSTR uname;
		usr->GetStringValue(PKEY_Identity_DisplayName, &uname);
		if (uname != NULL)
			users.push_back(uname);
	}
	StartWorker();
	return S_OK;
}
bool CSampleProvider::TryLogin(DWORD user, PWSTR pwd) {
	autouid = user;
	if (Credential != nullptr)
		Credential->Release();
	Credential = new(std::nothrow) CSampleCredential(this);
	if (Credential == nullptr)
        return true;
	ICredentialProviderUser* pCredUser;
	_pCredProviderUserArray->GetAt(selected_user, &pCredUser);
	if (pCredUser == NULL) {
		Credential->Release();
        return true;
	}
	Credential->Initialize(_cpus, pCredUser, pwd);
    if(!Credential->CheckLogin())return false;
	this->do_login = true;
	spcpe->CredentialsChanged(upAdviseCtx);
    return true;
}

HRESULT CSample_CreateInstance(_In_ REFIID riid, _Outptr_ void** ppv)
{
	HRESULT hr;
	CSampleProvider* pProvider = new(std::nothrow) CSampleProvider();
	if (pProvider)
	{
		hr = pProvider->QueryInterface(riid, ppv);
		pProvider->Release();
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}
	return hr;
}
void CSampleProvider::StartWorker() {
    if (hProcess != INVALID_HANDLE_VALUE)
		KillWorker();
	STARTUPINFOW si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sec;
	sec.nLength = sizeof(sec);
	sec.bInheritHandle = true;
	sec.lpSecurityDescriptor = NULL;

	auto sigDone = CreateEventW(&sec, FALSE, FALSE, NULL);
	sigReady = CreateEventW(&sec, FALSE, TRUE, NULL);

	uint64_t usrSize = 0;
	for (auto& usr : users) {
		usrSize += wcslen(usr.c_str()) + 1;
	}
	// User names here
	HANDLE pipe[4];
	CreatePipe(&pipe[0], &pipe[1], &sec, static_cast<DWORD>(sizeof(HANDLE) * 2 + sizeof(uint64_t) + usrSize * sizeof(wchar_t)));
	DWORD rSize;
	WriteFile(pipe[1], &sigDone, sizeof(HANDLE), &rSize, NULL);
	WriteFile(pipe[1], &sigReady, sizeof(HANDLE), &rSize, NULL);
	WriteFile(pipe[1], &usrSize, sizeof(uint64_t), &rSize, NULL);
	for (auto& usr : users) {
		auto ts = (wcslen(usr.c_str()) + 1) * sizeof(wchar_t);
		WriteFile(pipe[1], usr.c_str(), static_cast<DWORD>(ts), &rSize, NULL);
	}
	CloseHandle(pipe[1]);

	CreatePipe(&pipe[2], &pipe[3], &sec, 32);
	HANDLE hRead;
	DuplicateHandle(GetCurrentProcess(), pipe[2], GetCurrentProcess(), &hRead, NULL, FALSE, DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS);
	std::wstring exe = this->exePath;
    std::wstring cmd = L"\""+exe+L"\"" + L" " + std::to_wstring(reinterpret_cast<UINT_PTR>(pipe[0])) + L" " + std::to_wstring(reinterpret_cast<UINT_PTR>(pipe[3]));
	wchar_t* wcmd = new wchar_t[cmd.length() + 1];
	wcscpy_s(wcmd, cmd.length() + 1, cmd.c_str());
	CreateProcessW(exe.c_str(), wcmd,
		NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi
	);
	CloseHandle(pi.hThread);
	

	HANDLE h[2] = { pi.hProcess,sigDone };
	hProcess = pi.hProcess;

	worker = std::thread([=]() {
		while (1) {
			DWORD ret = WaitForMultipleObjects(2, h, FALSE, INFINITE);
			if (ret == WAIT_OBJECT_0 + 1) {
				DWORD uid=0;
				uint64_t pSize=0;
				DWORD rSize;
				BOOL ok = TRUE;
				if (ok) ok = ReadFile(hRead, &uid, sizeof(DWORD), &rSize, NULL);
				if (ok) ok = ReadFile(hRead, &pSize, sizeof(uint64_t), &rSize, NULL);
				if (ok && pSize > 2048) ok = false;
				if (ok) ok = ReadFile(hRead, &passwd, static_cast<DWORD>((pSize + 1) * sizeof(wchar_t)), &rSize, NULL);
				passwd[pSize] = L'\0';
				if (!ok) {
					isOk=false;
					KillWorker();
					return;
				}
				HANDLE rh;
                //BOOL logon = LogonUserW(users[uid].c_str(), L".", passwd, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &rh);
                //if (logon) {
                if(!TryLogin(uid, passwd)){
					SetEvent(sigReady);
                }
			}
		}
		});
	worker.detach();

}
void CSampleProvider::KillWorker() {

	if (hProcess == INVALID_HANDLE_VALUE) {
		return;
	}
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
	hProcess = INVALID_HANDLE_VALUE;
}
