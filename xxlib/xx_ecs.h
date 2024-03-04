#pragma once
#include "xx_list.h"

// some ref code here
//template<typename T, typename T::FieldId ID>
//struct Offseter {
//	static constexpr size_t offset();
//	std::string s;
//};
//
//template<typename T, typename T::FieldId ID>
//constexpr size_t Offseter<T, ID>::offset()
//{
//	return T::offset_of(std::integral_constant<decltype(ID), ID> {});
//}
//
//struct Foo {
//	enum FieldId { First, Second };
//	static constexpr size_t offset_of(std::integral_constant<FieldId, FieldId::First>) { return offsetof(Foo, first); }
//	static constexpr size_t offset_of(std::integral_constant<FieldId, FieldId::Second>) { return offsetof(Foo, second); }
//
//	int header;
//	Offseter<Foo, FieldId::First> first;
//	double interstitial;
//	Offseter<Foo, FieldId::Second> second;
//
//	virtual ~Foo() {}
//};
// 
//Foo o;
//constexpr auto offset1 = o.first.offset();
//constexpr auto offset2 = o.second.offset();


namespace xx {
	// B: base class
	// C: child / sub class
	// offset: index member memory address
	template<typename B, typename C, size_t offset>
	struct ECSIndex;

	template<typename B, typename C, size_t offset>
	struct ECSContainer {
		using Index = ECSIndex<B, C, offset>;

		// data + pointer
		struct Node : C {
			B* ptr{};
			Index& GetIndex() const {
				assert(ptr);
				return *(Index*)((char*)ptr + offset);
			}
		};

		// data + pointer array( can reserve, sort )
		xx::Listi<Node> nodes;

		// add
		void Attach(B* o, Index* ecsi);

		// remove
		void Detach(ptrdiff_t idx);

		// refill all index ( after sort ? )
		void UpdateIndexs();
	};

	template<typename B, typename C, size_t offset>
	struct ECSIndex {
		using Container = ECSContainer<B, C, offset>;

		// ref to container & data index
		Container* ptr{};
		ptrdiff_t idx{ -1 };

		// ref to data
		C& Get() const {
			return ptr->nodes[idx];
		}

		// attach
		void Init(Container& c, B* o) {
			c.Attach(o, this);
		}

		ECSIndex() = default;
		ECSIndex(ECSIndex const&) = delete;
		ECSIndex(ECSIndex&& o)
			: ptr(std::exchange(o.ptr, nullptr))
			, idx(std::exchange(o.idx, -1)) {}
		~ECSIndex() {
			if (ptr) {
				ptr->Detach(idx);
			}
			ptr = {};
			idx = -1;
		}
	};

	template<typename B, typename C, size_t offset>
	void ECSContainer<B, C, offset>::Attach(B* o, Index* ecsi) {
#ifndef NDEBUG
		auto p = (char*)ecsi - offset;
		assert(p == (char*)o);			// wrong offset value ?
#endif
		assert(o);
		assert(ecsi->ptr == nullptr);
		assert(ecsi->idx == -1);
		ecsi->ptr = this;
		ecsi->idx = nodes.len;
		auto& n = nodes.Emplace();
		n.Node::ptr = o;
	}

	template<typename B, typename C, size_t offset>
	void ECSContainer<B, C, offset>::Detach(ptrdiff_t idx) {
		nodes.SwapRemoveAt(idx);
		if (idx < nodes.len) {
			nodes[idx].GetIndex().idx = idx;
		}
	}

	template<typename B, typename C, size_t offset>
	void ECSContainer<B, C, offset>::UpdateIndexs() {
		for (ptrdiff_t i = 0, e = nodes.len; i < e; i++) {
			nodes[i].GetIndex().idx = i;
		}
	}
}

/*
// some example

struct A {
	float x, y;
}

struct B {
	std::string name, nick, desc;
}

struct Base {
	// no virtual here, ensure offset begin == 0. offset == *0, *2, *4, *6, ...
	
	xx::ECSIndex<Base, A, sizeof(size_t) * 0> a;
	xx::ECSIndex<Base, B, sizeof(size_t) * 2> b;
	// ...

	template<typename...ES>
	void Init(ES&...es) {
		a.Init( xx::GetAt<0>(es...), this );
		b.Init( xx::GetAt<1>(es...), this );
		// ...
	}
}

struct Foo : Base {
	// other member here... can virtual
	virtual ~Foo() {}
	// ...
}

inline void Test() {
	decltype(Base::a)::Container ecsA;
	decltype(Base::b)::Container ecsB;
	// ...

	xx::Listi<xx::Shared<Foo>> foos; 

	foos.Emplace().Emplace()->Init( ecsA, ecsB );
	// ...

	auto& ns = ecsA.nodes;
	std::sort(ns.buf, ns.buf + ns.len, [](auto& a, auto& b) { return a.y < b.y; });
	ecsA.UpdateIndexs();

	for (auto& node : ecsA.nodes) {
		// ...
	}

}

*/
