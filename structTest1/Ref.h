#pragma once

#include "xx_ptr.h"

template<typename T>
struct Ref {
	xx::PtrHeader* GetPtrHeader() const;

	// unsafe
	template<typename U = T>
	xx::Weak<U> WeakFromThis() const;

	template<typename U = T>
	xx::Shared<U> SharedFromThis() const;
};
