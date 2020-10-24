#include "DllWrapper.h"
#include "ClassFactory.h"
HRESULT DllWrapper::GetClassObject(__in REFCLSID rclsid, __in REFIID riid, __deref_out void** ppv) {
	return ClassFactory::CreateInstance(rclsid, riid, ppv);
}