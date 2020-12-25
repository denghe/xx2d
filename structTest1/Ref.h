#pragma once

#include "xx_ptr.h"

template<typename T>
struct Ref {
	XX_FORCEINLINE xx::PtrHeader* GetPtrHeader() const;

	// unsafe
	template<typename U = T>
	XX_FORCEINLINE xx::Weak<U> WeakFromThis() const;

	template<typename U = T>
	XX_FORCEINLINE xx::Shared<U> SharedFromThis() const;
};
