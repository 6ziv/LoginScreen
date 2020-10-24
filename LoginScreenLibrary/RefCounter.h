#pragma once
#include <Windows.h>
class RefCounter
{
private:
	long ref = 0;
public:
	inline RefCounter():ref(0){}
	inline ULONG AddRef() {
		InterlockedIncrement(&ref);
		return ref;
	}
	inline ULONG Release() {
		InterlockedDecrement(&ref);
		return ref;
	}
	inline bool CanUnload() {
		return ref == 0;
	}
};

