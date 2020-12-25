#pragma once

#include "Ref.h"

struct Signal : Ref<Signal> {
	std::string name;
	// bool 不能弄进去。否则 "asdf" 这种 literal string 会被认为是 bool 类型
	std::vector<std::variant<int32_t, int64_t, float, double, std::string>> args;
	/* ... todo more type */

	Signal() = default;

	template<typename Name, typename ...Args>
	Signal(Name&& name, Args&&... args);

	template<std::size_t I = 0, typename... Tp>
	std::enable_if_t<I == sizeof...(Tp), int> FillTuple_(std::tuple<Tp...>& t) const;

	template<std::size_t I = 0, typename... Tp>
	std::enable_if_t < I < sizeof...(Tp), int> FillTuple_(std::tuple<Tp...>& t) const;

	// args 的内容填充到 t. 出错返回非 0
	template<typename Tuple>
	int FillTuple(Tuple& t) const;
};


using MFuncHolder = std::array<size_t, 2>;
using MFuncLambda = void(*)(void* const&, MFuncHolder const&, Signal const&);
using MFuncData = std::pair<MFuncLambda, MFuncHolder>;
using MFuncMap = std::unordered_map<std::string_view, MFuncData>;

struct TypeInfo {
	MFuncMap funcs;
	// todo: props ....
};

template<typename T>
struct TypeInfoMappings {
	inline static TypeInfo typeInfo;
};


#define RegisterMethod_( T, funcName ) \
	RegisterMethod(__stringify(funcName), &T::funcName);

template<typename CT, typename FT>
inline void CallMFunc(CT* const& self, FT const& f, Signal const& s);

template<typename FN, typename FT>
inline void RegisterMethod(FN&& fn, FT&& f);


// todo: RegisterProperty


XX_HAS_TYPEDEF(AutoEnableProcess)
