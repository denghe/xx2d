#include <iostream>
#include <tuple>
#include <type_traits>

#define ENUM_PARAMS_0(x)
#define ENUM_PARAMS_1(x) x ## 1
#define ENUM_PARAMS_2(x) ENUM_PARAMS_1(x), x ## 2
#define ENUM_PARAMS_3(x) ENUM_PARAMS_2(x), x ## 3
#define ENUM_PARAMS_4(x) ENUM_PARAMS_3(x), x ## 4
#define ENUM_PARAMS_5(x) ENUM_PARAMS_4(x), x ## 5
#define ENUM_PARAMS_6(x) ENUM_PARAMS_5(x), x ## 6
#define ENUM_PARAMS_7(x) ENUM_PARAMS_6(x), x ## 7
#define ENUM_PARAMS_8(x) ENUM_PARAMS_7(x), x ## 8
#define ENUM_PARAMS_9(x) ENUM_PARAMS_8(x), x ## 9
#define ENUM_PARAMS_10(x) ENUM_PARAMS_9(x), x ## 10
#define ENUM_PARAMS_11(x) ENUM_PARAMS_10(x), x ## 11
#define ENUM_PARAMS_12(x) ENUM_PARAMS_11(x), x ## 12
#define ENUM_PARAMS_13(x) ENUM_PARAMS_12(x), x ## 13
#define ENUM_PARAMS_14(x) ENUM_PARAMS_13(x), x ## 14
#define ENUM_PARAMS_15(x) ENUM_PARAMS_14(x), x ## 15
#define ENUM_PARAMS_16(x) ENUM_PARAMS_15(x), x ## 16
#define ENUM_PARAMS(x, N) ENUM_PARAMS_##N(x)

#define ENUM_FOR_EACH_0(x)
#define ENUM_FOR_EACH_1(x) ENUM_FOR_EACH_0(x) x(1)
#define ENUM_FOR_EACH_2(x) ENUM_FOR_EACH_1(x) x(2)
#define ENUM_FOR_EACH_3(x) ENUM_FOR_EACH_2(x) x(3)
#define ENUM_FOR_EACH_4(x) ENUM_FOR_EACH_3(x) x(4)
#define ENUM_FOR_EACH_5(x) ENUM_FOR_EACH_4(x) x(5)
#define ENUM_FOR_EACH_6(x) ENUM_FOR_EACH_5(x) x(6)
#define ENUM_FOR_EACH_7(x) ENUM_FOR_EACH_6(x) x(7)
#define ENUM_FOR_EACH_8(x) ENUM_FOR_EACH_7(x) x(8)
#define ENUM_FOR_EACH_9(x) ENUM_FOR_EACH_8(x) x(9)
#define ENUM_FOR_EACH_10(x) ENUM_FOR_EACH_9(x) x(10)
#define ENUM_FOR_EACH_11(x) ENUM_FOR_EACH_10(x) x(11)
#define ENUM_FOR_EACH_12(x) ENUM_FOR_EACH_11(x) x(12)
#define ENUM_FOR_EACH_13(x) ENUM_FOR_EACH_12(x) x(13)
#define ENUM_FOR_EACH_14(x) ENUM_FOR_EACH_13(x) x(14)
#define ENUM_FOR_EACH_15(x) ENUM_FOR_EACH_14(x) x(15)
#define ENUM_FOR_EACH_16(x) ENUM_FOR_EACH_15(x) x(16)
#define ENUM_FOR_EACH(x, N) ENUM_FOR_EACH_##N(x)

namespace detail {
	template<std::size_t N> struct PriorityTag : PriorityTag<N - 1> {};
	template<> struct PriorityTag<0> {};

	template<class T> struct AnyConverter {
		template <class U, class = std::enable_if_t<!std::is_same<typename std::decay<T>::type, typename std::decay<U>::type>::value>>
		constexpr operator U ();
	};
	template<class T, std::size_t I> struct AnyConverterTag : AnyConverter<T> {};

	template<class T, class...Args> constexpr auto IsAggregateConstructibleImpl(T&&, Args &&...args) -> decltype(T{ std::forward<Args>(args)... }, std::true_type());
	template<class T, class Arg> constexpr auto IsAggregateConstructibleImpl(T&&, Arg&& args) -> decltype(T{ std::forward<Arg>(args) }, std::true_type());
	constexpr auto IsAggregateConstructibleImpl(...)->std::false_type;
	template<class T, class...Args> struct IsAggregateConstructible : decltype(IsAggregateConstructibleImpl(std::declval<T>(), std::declval<Args>()...)) {};

	template<class T, std::size_t...I> constexpr auto StructMemberCountImpl(std::index_sequence<I...>, PriorityTag<2>) -> std::enable_if_t<IsAggregateConstructible<T, AnyConverterTag<T, I>...>::value, std::size_t> { return sizeof...(I); }
	constexpr auto StructMemberCountImpl(std::index_sequence<>, PriorityTag<1>) -> std::size_t { return 0; }
	template<class T, class Seq> constexpr auto StructMemberCountImpl(Seq, PriorityTag<0>) -> std::size_t { return StructMemberCountImpl<T>(std::make_index_sequence<Seq::size() - 1>(), PriorityTag<2>()); };
	template<class T> constexpr auto StructMemberCountImpl() -> std::size_t { return StructMemberCountImpl<T>(std::make_index_sequence<sizeof(T)>(), PriorityTag<2>()); };
}

template<class T> struct StructMemberCount : std::integral_constant<std::size_t, detail::StructMemberCountImpl< std::remove_cv_t<std::remove_reference_t<T>> >()> {};

namespace detail {
#define APPLYER_DEF(N) \
    template<class T, class ApplyFunc> auto StructApply_impl(T &&in, ApplyFunc &&f, std::integral_constant<std::size_t, N>) { \
        auto &&[ENUM_PARAMS(x, N)] = std::forward<T>(in); \
        return f(ENUM_PARAMS(x, N)); \
    }; \

	ENUM_FOR_EACH(APPLYER_DEF, 16)
#undef APPLYER_DEF

	template<class T, class ApplyFunc> auto StructApply(T&& in, ApplyFunc&& f) {
		return StructApply_impl(std::forward<T>(in), std::forward<ApplyFunc>(f), StructMemberCount<T>());
	};
}

// StructToTuple : struct {A a, B b} => tuple<A, B>
template<class T> auto StructToTuple(T&& in) {
	return detail::StructApply(std::forward<T>(in), [](const auto&...args) { return std::make_tuple(args...); });
};

// TupleToStruct : tuple<A, B> => struct {A a, B b}
template<class T, class U> void TupleToStruct(T& out, U&& in) {
	detail::StructApply(out, [in = std::forward<U>(in)](auto&...args) { return std::tie(args...) = in; });
};


template<class T> void DumpStruct(T const& in) {
	std::apply([](auto const& ... args) { ((std::cout << args << " "), ...); }, StructToTuple(in));
	std::cout << std::endl;
}

int main()
{
	struct A { int v1; char v2; double v3; };
	A a1{ 1, '2', 3.0 };
	DumpStruct(a1);

	//auto&& t1 = StructToTuple(A{ 1, '2', 3.0 }); // std::tuple<int, char, double> a{1, '2', 3.0}
	//std::cout << std::get<0>(t1) << std::get<1>(t1) << std::get<2>(t1) << std::endl;
	//A a2; TupleToStruct(a2, std::make_tuple(4, '5', 6.0)); // a = A{4, '5', 6.0};
	//std::cout << a2.v1 << a2.v2 << a2.v3 << std::endl;
}
