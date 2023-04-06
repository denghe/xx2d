#pragma once
#include "xx_includes.h"

namespace xx {

	// double link + version's xx::ListLink but performance slow 3/5

	/*

#include <xx_listdoublelink.h>
int main() {
	int counter, n = 1, m = 5000000;
	auto secs = xx::NowEpochSeconds();

	counter = 0;
	for (size_t i = 0; i < n; i++) {

		xx::ListLink<int, int> ll;
		ll.Reserve(m);
		for (size_t j = 1; j <= m; j++) {
			new (&ll.Add()) int(j);
		}

		ll.Foreach([](auto& o)->bool {
			return o == 2 || o == 4;
		});
		new (&ll.Add()) int(2);
		new (&ll.Add()) int(4);

		ll.Foreach([&](auto& o) {
			counter += o;
		});
	}

	xx::CoutN("ListLink counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));

	counter = 0;
	for (size_t i = 0; i < n; i++) {

		xx::ListDoubleLink<int, int, int> ll;
		ll.Reserve(m);
		for (size_t j = 1; j <= m; j++) {
			new (&ll.At(ll.Add())) int(j);
		}

		ll.Foreach([](auto& o)->bool {
			return o == 2 || o == 4;
		});
		new (&ll.At(ll.Add())) int(2);
		new (&ll.At(ll.Add())) int(4);

		ll.Foreach([&](auto& o) {
			counter += o;
		});
	}

	xx::CoutN("ListDoubleLink counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));

	counter = 0;
	for (size_t i = 0; i < n; i++) {

		std::map<int, int> ll;
		int autoInc{};
		for (size_t j = 1; j <= m; j++) {
			ll[++autoInc] = j;
		}

		for (auto it = ll.begin(); it != ll.end();) {
			if (it->second == 2 || it->second == 4) {
				it = ll.erase(it);
			} else {
				++it;
			}
		}
		ll[++autoInc] = 2;
		ll[++autoInc] = 4;

		for (auto&& kv : ll) {
			counter += kv.second;
		}
	}

	xx::CoutN("map counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));


	counter = 0;
	for (size_t i = 0; i < n; i++) {

		std::list<int> ll;
		for (size_t j = 1; j <= m; j++) {
			ll.push_back(j);
		}

		for (auto it = ll.begin(); it != ll.end();) {
			if (*it == 2 || *it == 4) {
				it = ll.erase(it);
			} else {
				++it;
			}
		}
		ll.push_back(2);
		ll.push_back(4);

		for (auto&& v : ll) {
			counter += v;
		}
	}

	xx::CoutN("list counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));


	return 0;
}
	*/


	template<typename T, typename SizeType = ptrdiff_t, typename VersionType = size_t>
	struct ListDoubleLink {

		struct Node {
			SizeType next, prev;
			VersionType version;
			T value;
		};

		struct IndexAndVersion {
			SizeType index;
			VersionType version;
		};

		Node* buf{};
		SizeType cap{}, len{};
		SizeType head{ -1 }, tail{ -1 }, freeHead{ -1 }, freeCount{};
		VersionType version{};


		ListDoubleLink() = default;
		ListDoubleLink(ListDoubleLink const&) = delete;
		ListDoubleLink& operator=(ListDoubleLink const&) = delete;
		ListDoubleLink(ListDoubleLink&& o) {
			buf = o.buf;
			cap = o.cap;
			len = o.len;
			head = o.head;
			tail = o.tail;
			freeHead = o.freeHead;
			freeCount = o.freeCount;
			o.buf = {};
			o.cap = {};
			o.len = {};
			o.head = -1;
			o.tail = -1;
			o.freeHead = -1;
			o.freeCount = {};
		}
		ListDoubleLink& operator=(ListDoubleLink&& o) {
			std::swap(buf, o.buf);
			std::swap(cap, o.cap);
			std::swap(len, o.len);
			std::swap(head, o.head);
			std::swap(tail, o.tail);
			std::swap(freeHead, o.freeHead);
			std::swap(freeCount, o.freeCount);
			return *this;
		}
		~ListDoubleLink() {
			Clear<true>();
		}

		void Reserve(SizeType const& newCap) noexcept {
			assert(newCap > 0);
			if (newCap <= cap) return;
			cap = newCap;
			if constexpr (IsPod_v<T>) {
				buf = (Node*)realloc(buf, sizeof(Node) * newCap);
			} else {
				auto newBuf = (Node*)::malloc(newCap * sizeof(Node));
				for (SizeType i = 0; i < len; ++i) {
					newBuf[i].prev = buf[i].prev;
					newBuf[i].next = buf[i].next;
					newBuf[i].version = buf[i].version;
					new (&newBuf[i].value) T((T&&)buf[i].value);
					buf[i].value.~T();
				}
				::free(buf);
				buf = newBuf;
			}
		}

		// return value: new ( &ll.At( ll.Add() ) ) T( ... );
		template<bool add_to_tail = true>
		IndexAndVersion Add() {

			SizeType idx;
			if (freeCount > 0) {
				idx = freeHead;
				freeHead = buf[idx].next;
				freeCount--;
			} else {
				if (len == cap) {
					Reserve(cap ? cap * 2 : 8);
				}
				idx = len;
				len++;
			}

			if constexpr (add_to_tail) {
				buf[idx].prev = tail;
				buf[idx].next = -1;

				if (tail >= 0) {
					buf[tail].next = idx;
					tail = idx;
				} else {
					assert(head == -1);
					head = tail = idx;
				}
			} else {
				buf[idx].prev = -1;
				buf[idx].next = head;

				if (head >= 0) {
					buf[head].prev = idx;
					head = idx;
				} else {
					assert(head == -1);
					head = tail = idx;
				}
			}

			buf[idx].version = ++version;
			return { idx, version };
		}

		bool Exists(SizeType const& idx) const {
			assert(idx >= 0);
			assert(idx < len);
			return buf[idx].version >= 0;
		}
		bool Exists(IndexAndVersion const& iv) const {
			assert(iv.index >= 0);
			assert(iv.index < len);
			return buf[iv.index].version == iv.version;
		}

		void Remove(SizeType const& idx) {
			assert(Exists(idx));

			auto& node = buf[idx];
			auto prev = node.prev;
			auto next = node.next;
			if (idx == head) {
				head = next;
			}
			if (idx == tail) {
				tail = prev;
			}
			if (prev >= 0) {
				buf[prev].next = next;
			}
			if (next >= 0) {
				buf[next].prev = prev;
			}

			node.value.~T();
			node.next = freeHead;
			node.prev = -1;
			node.version = 0;
			freeHead = idx;
			++freeCount;
		}
		bool Remove(IndexAndVersion const& iv) {
			if (!Exists(iv)) return false;
			Remove(iv.index);
			return true;
		}

		IndexAndVersion ToIndexAndVersion(SizeType const& idx) const {
			return { idx, buf[idx].version };
		}

		SizeType Next(SizeType const& idx) const {
			assert(Exists(idx));
			return buf[idx].next;
		}

		SizeType Prev(SizeType const& idx) const {
			assert(Exists(idx));
			return buf[idx].prev;
		}

		T const& At(SizeType const& idx) const {
			assert(Exists(idx));
			return buf[idx].value;
		}

		T& At(SizeType const& idx) {
			assert(Exists(idx));
			return buf[idx].value;
		}

		T const& At(IndexAndVersion const& iv) const {
			assert(Exists(iv));
			return buf[iv.index].value;
		}

		T& At(IndexAndVersion const& iv) {
			assert(Exists(iv));
			return buf[iv.index].value;
		}



		template<bool freeBuf = false>
		void Clear() {

			if (!cap) return;
			if constexpr (IsPod_v<T>) {
				while (head >= 0) {
					buf[head].value.~T();
					head = buf[head].next;
				}
			}
			if constexpr (freeBuf) {
				::free(buf);
				buf = {};
				cap = 0;
			}

			head = tail = freeHead = -1;
			freeCount = 0;
			len = 0;
		}

		T const& operator[](SizeType const& idx) const noexcept {
			assert(Exists(idx));
			return buf[idx].value;
		}

		T& operator[](SizeType const& idx) noexcept {
			assert(Exists(idx));
			return buf[idx].value;
		}

		T const& operator[](IndexAndVersion const& iv) const noexcept {
			assert(Exists(iv));
			return buf[iv.index].value;
		}

		T& operator[](IndexAndVersion const& iv) noexcept {
			assert(Exists(iv));
			return buf[iv.index].value;
		}

		SizeType Left() const {
			return cap - len + freeCount;
		}

		SizeType Count() const {
			return len - freeCount;
		}

		bool Empty() const {
			return len - freeCount == 0;
		}

		template<typename F>
		void Foreach(F&& f) {
			if constexpr (std::is_void_v<decltype(f(buf[0].value))>) {
				for (auto idx = head; idx != -1; idx = Next(idx)) {
					f(buf[idx].value);
				}
			} else {
				for (SizeType next, idx = head; idx != -1;) {
					next = Next(idx);
					if (f(buf[idx].value)) {
						Remove(idx);
					}
					idx = next;
				}
			}
		}

		template<typename F>
		void ForeachReverse(F&& f) {
			if constexpr (std::is_void_v<decltype(f(buf[0].value))>) {
				for (auto idx = tail; idx != -1; idx = Prev(idx)) {
					f(buf[idx].value);
				}
			} else {
				for (SizeType prev, idx = tail; idx != -1;) {
					prev = Prev(idx);
					if (f(buf[idx].value)) {
						Remove(idx);
					}
					idx = prev;
				}
			}
		}
	};
}
