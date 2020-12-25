#pragma once

template<typename Name, typename ...Args>
inline Signal::Signal(Name&& name, Args&&... args) {
	this->name = std::forward<Name>(name);
	((this->args.emplace_back(std::forward<Args>(args)), 0), ...);
}


template<std::size_t I, typename... Tp>
XX_FORCEINLINE std::enable_if_t<I == sizeof...(Tp), int> Signal::FillTuple_(std::tuple<Tp...>& t) const {
	return 0;
}

template<std::size_t I, typename... Tp>
XX_FORCEINLINE std::enable_if_t < I < sizeof...(Tp), int> Signal::FillTuple_(std::tuple<Tp...>& t) const {
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
int Signal::FillTuple(Tuple& t) const {
	if (std::tuple_size_v<Tuple> != args.size()) return __LINE__;
	FillTuple_(t);
	return 0;
}

template<typename CT, typename FT>
inline void CallMFunc(CT* const& self, FT const& f, Signal const& s) {
	xx::FuncA_t<FT> tuple;
	if (s.FillTuple(tuple)) return;
	std::apply([&](auto &... args) {
		(self->*f)(args...);
		}, tuple);
}

template<typename FN, typename FT>
inline void RegisterMethod(FN&& fn, FT&& f) {
	static_assert(sizeof(f) == sizeof(MFuncHolder));
	using CT = xx::FuncC_t<FT>;
	TypeInfoMappings<CT>::typeInfo.funcs[fn] = std::make_pair([](void* const& self, MFuncHolder const& f, Signal const& s) {
		CallMFunc<CT, FT>((CT*)self, *(FT*)f.data(), s);
		}, *(MFuncHolder*)&f);
}
