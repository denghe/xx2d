#pragma once
#include "xx_ptr.h"	// xx::Shared xx::Weak

template<typename T>
struct Ref {
	XX_FORCEINLINE xx::PtrHeader* GetPtrHeader() const {
		return (xx::PtrHeader*)(T*)this - 1;
	}

	// unsafe
	template<typename U = T>
	XX_FORCEINLINE xx::Weak<U> WeakFromThis() const {
		auto h = GetPtrHeader();
		assert((*(xx::Weak<U>*) & h).Lock().As<U>());
		return *(xx::Weak<U>*) & h;
	}

	template<typename U = T>
	XX_FORCEINLINE xx::Shared<U> SharedFromThis() const {
		return WeakFromThis<U>().Lock();
	}
};
