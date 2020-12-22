#pragma once
#include "Ref.h"
#include <variant>

struct Signal : Ref<Signal> {
	std::string name;
	// bool 不能弄进去。否则 "asdf" 这种 literal string 会被认为是 bool 类型
	std::vector<std::variant<int32_t, int64_t, float, double, std::string>> args;
	/* ... todo more type */

	Signal() = default;

	template<typename Name, typename ...Args>
	Signal(Name&& name, Args&&... args) {
		this->name = std::forward<Name>(name);
		((this->args.emplace_back(std::forward<Args>(args)), 0), ...);
	}


	template<std::size_t I = 0, typename... Tp>
	XX_FORCEINLINE std::enable_if_t<I == sizeof...(Tp), int> FillTuple_(std::tuple<Tp...>& t) const {
		return 0;
	}

	template<std::size_t I = 0, typename... Tp>
	XX_FORCEINLINE std::enable_if_t < I < sizeof...(Tp), int> FillTuple_(std::tuple<Tp...>& t) const {
		auto& a = std::get<I>(t);
		typedef std::tuple_element_t<I, std::tuple<Tp...>> AT;
		auto& b = args[I];
		if constexpr (std::is_same_v<std::string, AT> || std::is_same_v<std::string_view, AT>) {
			if (b.index() != 4) return __LINE__;
			a = std::get<std::string>(b);
		}
		else {
			switch (b.index()) {
			case 0:
				a = (AT)std::get<int32_t>(b);
				break;
			case 1:
				a = (AT)std::get<int64_t>(b);
				break;
			case 2:
				a = (AT)std::get<float>(b);
				break;
			case 3:
				a = (AT)std::get<double>(b);
				break;
			default:
				return __LINE__;
			}
		}
		return FillTuple_<I + 1, Tp...>(t);
	}

	// args 的内容填充到 t. 出错返回非 0
	template<typename Tuple>
	int FillTuple(Tuple& t) const {
		if (std::tuple_size_v<Tuple> != args.size()) return __LINE__;
		FillTuple_(t);
		return 0;
	}
};


using MFuncHolder = std::array<size_t, 2>;
typedef void(*MFuncLambda)(void* const&, MFuncHolder const&, Signal const&);
using MFuncMap = std::unordered_map<std::string_view, std::pair<MFuncLambda, MFuncHolder>>;

// 为每个类型声明一个 函数名 & 调用lambda 映射
template<typename T>
struct TypeMFuncMappings {
	inline static MFuncMap map;
};

#define __stringify(x)  __stringify_1(x)
#define __stringify_1(x)  #x
#define RegisterMethod( T, funcName ) \
	RegisterMethod_Impl(__stringify(funcName), &T::funcName);

template<typename CT, typename FT>
inline void CallMFunc(CT* const& self, FT const& f, Signal const& s) {
	xx::FuncA_t<FT> tuple;
	if (s.FillTuple(tuple)) return;
	std::apply([&](auto &... args) {
		(self->*f)(args...);
		}, tuple);
}

template<typename FN, typename FT>
inline void RegisterMethod_Impl(FN&& fn, FT&& f) {
	static_assert(sizeof(f) == sizeof(MFuncHolder));
	using CT = xx::FuncC_t<FT>;
	TypeMFuncMappings<CT>::map[fn] = std::make_pair([](void* const& self, MFuncHolder const& f, Signal const& s) {
		CallMFunc<CT, FT>((CT*)self, *(FT*)f.data(), s);
		}, *(MFuncHolder*)&f);
}
