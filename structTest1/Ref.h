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

namespace xx {
	template<typename T>
	struct PtrHeaderSwitcher<T, std::enable_if_t< XX_IsTemplateOf(Ref, T)::value >> {
		using type = PtrHeader;
	};
}
