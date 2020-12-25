#pragma once

template<typename T>
XX_FORCEINLINE xx::PtrHeader* Ref<T>::GetPtrHeader() const {
	return (xx::PtrHeader*)(T*)this - 1;
}

// unsafe
template<typename T>
template<typename U>
XX_FORCEINLINE xx::Weak<U> Ref<T>::WeakFromThis() const {
	auto h = GetPtrHeader();
	return *(xx::Weak<U>*) & h;
}

template<typename T>
template<typename U>
XX_FORCEINLINE xx::Shared<U> Ref<T>::SharedFromThis() const {
	return WeakFromThis<U>().Lock();
}
