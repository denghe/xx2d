#pragma once
#include "xx_includes.h"

namespace xx {

	// fast add/remove container. can visit members by Add order ( faster than map 10+ times )

	/*
	* example: ( more example at : xx_listdoublelink.h )

#include <xx_listlink.h>

int main() {

	int counter = 0, n = 1, m = 5000000;
	auto secs = xx::NowEpochSeconds();

	for (size_t i = 0; i < n; i++) {

		xx::ListLink<int, int> ll;
		ll.Reserve(m);
		for (size_t j = 1; j <= m; j++) {
			new (&ll.Add()) int(j);
		}

		int prev = -1, next{};
		for (auto idx = ll.head; idx != -1;) {
			if (ll[idx] == 2 || ll[idx] == 4) {
				next = ll.Remove(idx, prev);
			} else {
				next = ll.Next(idx);
				prev = idx;
			}
			idx = next;
		}
		new (&ll.Add()) int(2);
		new (&ll.Add()) int(4);

		for (auto idx = ll.head; idx != -1; idx = ll.Next(idx)) {
			counter += ll[idx];
		}
	}

	xx::CoutN("ListLink counter = ", counter, " secs = ", xx::NowEpochSeconds(secs));

	counter = 0;
	for (size_t i = 0; i < n; i++) {

		std::map<int, int> ll;
		int autoInc{};
		for (size_t j = 1; j <= m; j++) {
			ll[++autoInc] = j;
		}

		for(auto it = ll.begin(); it != ll.end();) {
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

	return 0;
}

	
	*/

	template<typename T, typename SizeType = ptrdiff_t>
	struct ListLink {

		struct Node {
			SizeType next;
			T value;
		};

		Node* buf{};
		SizeType cap{}, len{};
		SizeType head{ -1 }, tail{ -1 }, freeHead{ -1 }, freeCount{};

		ListLink() = default;
		ListLink(ListLink const&) = delete;
		ListLink& operator=(ListLink const&) = delete;
		ListLink(ListLink&& o) {
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
		ListLink& operator=(ListLink&& o) {
			std::swap(buf, o.buf);
			std::swap(cap, o.cap);
			std::swap(len, o.len);
			std::swap(head, o.head);
			std::swap(tail, o.tail);
			std::swap(freeHead, o.freeHead);
			std::swap(freeCount, o.freeCount);
			return *this;
		}
		~ListLink() {
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
					newBuf[i].next = buf[i].next;
					new (&newBuf[i].value) T((T&&)buf[i].value);
					buf[i].value.~T();
				}
				::free(buf);
				buf = newBuf;
			}
		}

		// return value: new ( &ll.Add() ) T( ... );
		T& Add() {
			SizeType idx;
			if (freeCount > 0) {
				idx = freeHead;
				freeHead = buf[idx].next;
				freeCount--;
			} else {
				if (len == cap) {
					Reserve(cap ? cap * 2 : 64);
				}
				idx = len;
				len++;
			}

			buf[idx].next = -1;

			if (tail >= 0) {
				buf[tail].next = idx;
				tail = idx;
			} else {
				assert(head == -1);
				head = tail = idx;
			}

			return buf[idx].value;
		}

		// return next index
		SizeType Remove(SizeType const& idx, SizeType const& prevIdx = -1) {
			assert(idx >= 0);
			assert(idx < len);

			if (idx == head) {
				head = buf[idx].next;
			}
			if (idx == tail) {
				tail = prevIdx;
			}
			auto r = buf[idx].next;
			if (prevIdx >= 0) {
				buf[prevIdx].next = r;
			}
			buf[idx].value.~T();
			buf[idx].next = freeHead;
			freeHead = idx;
			++freeCount;
			return r;
		}

		SizeType Next(SizeType const& idx) const {
			return buf[idx].next;
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
			assert(idx >= 0);
			assert(idx < len);
			return buf[idx].value;
		}

		T& operator[](SizeType const& idx) noexcept {
			assert(idx >= 0);
			assert(idx < len);
			return buf[idx].value;
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

		// maybe slowly than direct for?
		template<typename F>
		void Foreach(F&& f) {
			if constexpr (std::is_void_v<decltype(f(buf[0].value))>) {
				for (auto idx = head; idx != -1; idx = Next(idx)) {
					f(buf[idx].value);
				}
			} else {
				for (SizeType prev = -1, next, idx = head; idx != -1;) {
					if (f(buf[idx].value)) {
						next = Remove(idx, prev);
					} else {
						next = Next(idx);
						prev = idx;
					}
					idx = next;
				}
			}
		}
	};
}
