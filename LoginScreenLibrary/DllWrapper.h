#pragma once
#include "RefCounter.h"
#include <memory>
class DllWrapper:public RefCounter
{
private:
	inline DllWrapper():RefCounter(){}
	inline static DllWrapper* instance=nullptr;
public:
	inline static DllWrapper* GetInstance() {
		if (instance == nullptr)
			instance = new (std::nothrow) DllWrapper;
		return instance;
	}
	inline static void FreeInstance() {
		if(instance)
			delete instance;
		instance = nullptr;
	}
	HMODULE hinst;
	HRESULT GetClassObject(__in REFCLSID rclsid, __in REFIID riid, __deref_out void** ppv);
};

