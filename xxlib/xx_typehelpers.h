#pragma once

#include <type_traits>
#include <cstddef>
#include <optional>
#include <vector>
#include <string>
#include <variant>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <ctime>  // std::tm
#include <iomanip>  // std::put_time
#include <array>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <thread>


#ifndef NDEBUG
#define XX_NOINLINE
#define XX_FORCEINLINE
#else
#ifdef _MSC_VER
#define XX_NOINLINE __declspec(noinline)
#define XX_FORCEINLINE __forceinline
#else
#define XX_NOINLINE __attribute__((noinline))
#define XX_FORCEINLINE __attribute__((always_inline))
#endif
#endif
#define XX_INLINE inline


#define XX_STRINGIFY(x)  XX_STRINGIFY_(x)
#define XX_STRINGIFY_(x)  #x

#define XX_HAS_TYPEDEF( TN ) \
template<typename, typename = void> struct HasTypedef_##TN : std::false_type {}; \
template<typename T> struct HasTypedef_##TN<T, std::void_t<typename T::TN>> : std::true_type {}; \
template<typename T> constexpr bool TN = HasTypedef_##TN<T>::value;


namespace xx {
	/************************************************************************************/
	// std::is_pod 的自定义扩展, 用于标识一个类可以在容器中被r memcpy | memmove

	template<typename T, typename ENABLED = void>
	struct IsPod : std::false_type {
	};
	template<typename T>
	struct IsPod<T, std::enable_if_t<std::is_standard_layout_v<T>&& std::is_trivial_v<T>>> : std::true_type {
	};
	template<typename T>
	constexpr bool IsPod_v = IsPod<T>::value;


	/************************************************************************************/
	// 模板类型识别系列


	template<typename T, typename = void>
	struct IsLiteral : std::false_type {
	};
	template<size_t L>
	struct IsLiteral<char[L], void> : std::true_type {
	};
	template<size_t L>
	struct IsLiteral<char const [L], void> : std::true_type {
	};
	template<size_t L>
	struct IsLiteral<char const (&)[L], void> : std::true_type {
	};
	template<typename T>
	constexpr bool IsLiteral_v = IsLiteral<T>::value;


	template<typename T>
	struct IsTimePoint : std::false_type {
	};
	template<typename C, typename D>
	struct IsTimePoint<std::chrono::time_point<C, D>> : std::true_type {
	};
	template<typename C, typename D>
	struct IsTimePoint<std::chrono::time_point<C, D>&> : std::true_type {
	};
	template<typename C, typename D>
	struct IsTimePoint<std::chrono::time_point<C, D>const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsTimePoint_v = IsTimePoint<T>::value;


	template<typename T>
	struct IsOptional : std::false_type {
	};
	template<typename T>
	struct IsOptional<std::optional<T>> : std::true_type {
	};
	template<typename T>
	struct IsOptional<std::optional<T>&> : std::true_type {
	};
	template<typename T>
	struct IsOptional<std::optional<T> const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsOptional_v = IsOptional<T>::value;


	template<typename T>
	struct IsVector : std::false_type {
	};
	template<typename T>
	struct IsVector<std::vector<T>> : std::true_type {
	};
	template<typename T>
	struct IsVector<std::vector<T>&> : std::true_type {
	};
	template<typename T>
	struct IsVector<std::vector<T> const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsVector_v = IsVector<T>::value;


	template<typename>
	struct IsTuple : std::false_type {
	};
	template<typename ...T>
	struct IsTuple<std::tuple<T...>> : std::true_type {
	};
	template<typename ...T>
	struct IsTuple<std::tuple<T...>&> : std::true_type {
	};
	template<typename ...T>
	struct IsTuple<std::tuple<T...> const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsTuple_v = IsTuple<T>::value;


	template<typename T>
	struct IsShared : std::false_type {
	};
	template<typename T>
	struct IsShared<std::shared_ptr<T>> : std::true_type {
	};
	template<typename T>
	struct IsShared<std::shared_ptr<T>&> : std::true_type {
	};
	template<typename T>
	struct IsShared<std::shared_ptr<T> const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsShared_v = IsShared<T>::value;


	template<typename T>
	struct IsWeak : std::false_type {
	};
	template<typename T>
	struct IsWeak<std::weak_ptr<T>> : std::true_type {
	};
	template<typename T>
	struct IsWeak<std::weak_ptr<T>&> : std::true_type {
	};
	template<typename T>
	struct IsWeak<std::weak_ptr<T> const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsWeak_v = IsWeak<T>::value;


	template<typename T>
	struct IsUnique : std::false_type {
	};
	template<typename T>
	struct IsUnique<std::unique_ptr<T>> : std::true_type {
	};
	template<typename T>
	struct IsUnique<std::unique_ptr<T>&> : std::true_type {
	};
	template<typename T>
	struct IsUnique<std::unique_ptr<T> const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsUnique_v = IsUnique<T>::value;


	template<typename T>
	struct IsUnorderedSet : std::false_type {
	};
	template<typename T>
	struct IsUnorderedSet<std::unordered_set<T>> : std::true_type {
	};
	template<typename T>
	struct IsUnorderedSet<std::unordered_set<T>&> : std::true_type {
	};
	template<typename T>
	struct IsUnorderedSet<std::unordered_set<T> const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsUnorderedSet_v = IsUnorderedSet<T>::value;


	template<typename T>
	struct IsUnorderedMap : std::false_type {
	};
	template<typename K, typename V>
	struct IsUnorderedMap<std::unordered_map<K, V>> : std::true_type {
		typedef std::pair<K, V> PT;
	};
	template<typename K, typename V>
	struct IsUnorderedMap<std::unordered_map<K, V>&> : std::true_type {
		typedef std::pair<K, V> PT;
	};
	template<typename K, typename V>
	struct IsUnorderedMap<std::unordered_map<K, V> const&> : std::true_type {
		typedef std::pair<K, V> PT;
	};
	template<typename T>
	constexpr bool IsUnorderedMap_v = IsUnorderedMap<T>::value;
	template<typename T>
	using UnorderedMap_Pair_t = typename IsUnorderedMap<T>::PT;


	template<typename T>
	struct IsMap : std::false_type {
	};
	template<typename K, typename V>
	struct IsMap<std::map<K, V>> : std::true_type {
		typedef std::pair<K, V> PT;
	};
	template<typename K, typename V>
	struct IsMap<std::map<K, V>&> : std::true_type {
		typedef std::pair<K, V> PT;
	};
	template<typename K, typename V>
	struct IsMap<std::map<K, V> const&> : std::true_type {
		typedef std::pair<K, V> PT;
	};
	template<typename T>
	constexpr bool IsMap_v = IsMap<T>::value;
	template<typename T>
	using Map_Pair_t = typename IsMap<T>::PT;


	template<typename T>
	struct IsPair : std::false_type {
	};
	template<typename F, typename S>
	struct IsPair<std::pair<F, S>> : std::true_type {
	};
	template<typename F, typename S>
	struct IsPair<std::pair<F, S>&> : std::true_type {
	};
	template<typename F, typename S>
	struct IsPair<std::pair<F, S>const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsPair_v = IsPair<T>::value;


	template<typename T>
	struct IsArray : std::false_type {
	};
	template<typename T, size_t S>
	struct IsArray<std::array<T, S>> : std::true_type {
	};
	template<typename T, size_t S>
	struct IsArray<std::array<T, S>&> : std::true_type {
	};
	template<typename T, size_t S>
	struct IsArray<std::array<T, S>const&> : std::true_type {
	};
	template<typename T>
	constexpr bool IsArray_v = IsArray<T>::value;


	template<typename, typename = void>
	struct IsContainer : std::false_type {
	};
	template<typename T>
	struct IsContainer<T, std::void_t<decltype(std::declval<T>().data()), decltype(std::declval<T>().size())>>
		: std::true_type {
	};


	/************************************************************************************/
	// IsFunction_v  FunctionType_t 引用类型参数容器类型路由

	template<typename T>
	struct IsFunction : std::false_type {
	};
	template<typename T>
	struct IsFunction<std::function<T>> : std::true_type {
		using FT = T;
	};
	template<typename T>
	struct IsFunction<std::function<T>&> : std::true_type {
		using FT = T;
	};
	template<typename T>
	struct IsFunction<std::function<T> const&> : std::true_type {
		using FT = T;
	};
	template<typename T>
	constexpr bool IsFunction_v = IsFunction<T>::value;
	template<typename T>
	using FunctionType_t = typename IsFunction<T>::FT;


	/************************************************************************************/
	// IsPointerClass_v  是否为指针类 T*, shared_ptr, unique_ptr
	// ToPointer(T?? v) 返回指针版 v

	template<typename, typename = void>
	struct IsPointerClass : std::false_type {
	};

	template<typename T>
	struct IsPointerClass<T, std::enable_if_t<std::is_pointer_v<std::decay_t<T>> || IsShared_v<T> || IsUnique_v<T>>>
		: std::true_type {
	};

	template<typename T>
	constexpr bool IsPointerClass_v = IsPointerClass<T>::value;

	template<typename T, class = void>
	struct ToPointerFuncs;

	template<typename T>
	struct ToPointerFuncs<T, std::enable_if_t<IsPointerClass_v<T>>> {
		static inline auto Convert(T&& v) {
			return &*v;
		}
	};

	template<typename T>
	struct ToPointerFuncs<T, std::enable_if_t<!IsPointerClass_v<T> && !IsWeak_v<T>>> {
		static inline std::remove_reference_t<T>* Convert(T&& v) {
			return &v;
		}
	};

	template<typename T>
	struct ToPointerFuncs<T, std::enable_if_t<IsWeak_v<T>>> {
		static inline auto Convert(T&& v) {
			return v.lock();
		}
	};

	template<typename T>
	auto ToPointer(T&& v) {
		return ToPointerFuncs<T>::Convert(std::forward<T>(v));
	}

	/************************************************************************************/
	// RefC_t 引用类型参数容器类型路由

	template<typename T>
	struct RefWrapper {
		T* p = nullptr;

		RefWrapper() = default;

		RefWrapper(T& v) : p(&v) {}

		inline operator T& () { return *p; }
	};

	template<typename T, class = void>
	struct RefTraits {
		using C = std::decay_t<T>;
	};

	template<typename T>
	struct RefTraits<T, std::enable_if_t<std::is_reference_v<T> && !std::is_const_v<std::remove_reference_t<T>>>> {
		using C = RefWrapper<std::decay_t<T>>;
	};

	template<typename T>
	using RefC_t = typename RefTraits<T>::C;


	/************************************************************************************/
	// FuncR_t   FuncA_t  FuncC_t  lambda / function 类型拆解

	template<typename T, class = void>
	struct FuncTraits;

	template<typename Rtv, typename...Args>
	struct FuncTraits<Rtv(*)(Args ...)> {
		using R = Rtv;
		using A = std::tuple<RefC_t<Args>...>;
		using C = void;
	};

	template<typename Rtv, typename...Args>
	struct FuncTraits<Rtv(Args ...)> {
		using R = Rtv;
		using A = std::tuple<RefC_t<Args>...>;
		using C = void;
	};

	template<typename Rtv, typename CT, typename... Args>
	struct FuncTraits<Rtv(CT::*)(Args ...)> {
		using R = Rtv;
		using A = std::tuple<RefC_t<Args>...>;
		using C = CT;
	};

	template<typename Rtv, typename CT, typename... Args>
	struct FuncTraits<Rtv(CT::*)(Args ...) const> {
		using R = Rtv;
		using A = std::tuple<RefC_t<Args>...>;
		using C = CT;
	};

	template<typename T>
	struct FuncTraits<T, std::void_t<decltype(&T::operator())> >
		: public FuncTraits<decltype(&T::operator())> {
	};

	template<typename T>
	using FuncR_t = typename FuncTraits<T>::R;
	template<typename T>
	using FuncA_t = typename FuncTraits<T>::A;
	template<typename T>
	using FuncC_t = typename FuncTraits<T>::C;


	/************************************************************************************/
	// MemberPointerRtv_t   MemberPointerClass_t  成员变量指针 类型拆解

	template<typename T>
	struct MemberPointerTraits;

	template<typename Rtv, typename CT>
	struct MemberPointerTraits<Rtv(CT::*)> {
		using R = Rtv;
		using C = CT;
	};

	template<typename T>
	using MemberPointerR_t = typename MemberPointerTraits<T>::R;
	template<typename T>
	using MemberPointerC_t = typename MemberPointerTraits<T>::C;


	/************************************************************************************/
	// 容器子类型检测相关

	template<typename T>
	struct ChildType {
		using type = void;
	};
	template<typename T>
	using ChildType_t = typename ChildType<T>::type;


	template<typename T>
	struct ChildType<std::optional<T>> {
		using type = T;
	};
	template<typename T>
	struct ChildType<std::vector<T>> {
		using type = T;
	};

	/************************************************************************************/
	// shared_ptr 系列

	template<typename T, typename ...Args>
	std::shared_ptr<T> Make(Args &&...args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ...Args>
	std::shared_ptr<T> TryMake(Args &&...args) noexcept {
		try {
			return std::make_shared<T>(std::forward<Args>(args)...);
		}
		catch (...) {
			return std::shared_ptr<T>();
		}
	}

	template<typename T, typename ...Args>
	std::shared_ptr<T>& MakeTo(std::shared_ptr<T>& v, Args &&...args) {
		v = std::make_shared<T>(std::forward<Args>(args)...);
		return v;
	}

	template<typename T, typename ...Args>
	std::shared_ptr<T>& TryMakeTo(std::shared_ptr<T>& v, Args &&...args) noexcept {
		v = TryMake<T>(std::forward<Args>(args)...);
		return v;
	}

	template<typename T, typename U>
	std::shared_ptr<T> As(std::shared_ptr<U> const& v) noexcept {
		return std::dynamic_pointer_cast<T>(v);
	}

	template<typename T, typename U>
	bool Is(std::shared_ptr<U> const& v) noexcept {
		return std::dynamic_pointer_cast<T>(v) != nullptr;
	}

	/************************************************************************************/
	// weak_ptr 系列

	template<typename T, typename U>
	std::weak_ptr<T> AsWeak(std::shared_ptr<U> const& v) noexcept {
		return std::weak_ptr<T>(As<T>(v));
	}

	template<typename T>
	std::weak_ptr<T> ToWeak(std::shared_ptr<T> const& v) noexcept {
		return std::weak_ptr<T>(v);
	}



	/************************************************************************************/
	// unique_ptr 系列

	template<typename T, typename ...Args>
	std::unique_ptr<T> MakeU(Args &&...args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ...Args>
	std::unique_ptr<T> TryMakeU(Args &&...args) noexcept {
		try {
			return std::make_unique<T>(std::forward<Args>(args)...);
		}
		catch (...) {
			return std::unique_ptr<T>();
		}
	}


	/************************************************************************************/
	// malloc 系列

	template<typename T>
	T* Malloc() {
		return (T*)malloc(sizeof(T));
	}

	template<typename T>
	T*& MallocTo(T*& v) {
		v = (T*)malloc(sizeof(T));
		return v;
	}



	/************************************************************************************/
	// TypeId 映射
	template<typename T>
	struct TypeId {
		static const uint16_t value = 0;
	};

	template<typename T>
	constexpr uint16_t TypeId_v = TypeId<T>::value;

	/*
	// 方便复制

namespace xx {
	template<>
	struct TypeId<XXXXXXXXXXXXXX> {
		static const uint16_t value = XXXXXXXX;
	};
}
*/


	/************************************************************************************/
	// TypeName_v

	namespace Detail {
		template<typename... T>
		constexpr auto NameOf() noexcept {
#if defined(__clang__)
			/* 输出前后缀：
auto xx::Detail::NameOf() [T = <
>]
*/
			return std::string_view{ __PRETTY_FUNCTION__ + 32, sizeof(__PRETTY_FUNCTION__) - 35 };
#elif defined(__GNUC__)
			/* 输出前后缀：
constexpr auto xx::Detail::NameOf() [with T = {
}]
*/
			return std::string_view{ __PRETTY_FUNCTION__ + 47, sizeof(__PRETTY_FUNCTION__) - 50 };
#elif defined(_MSC_VER) && _MSC_VER >= 1920
			/* 输出前后缀：
auto __cdecl xx::Detail::NameOf<
>(void) noexcept
*/
			return std::string_view{ __FUNCSIG__ + 32, sizeof(__FUNCSIG__) - 49 };
#else
			static_assert(false, "unsupported compiler");
#endif
		}
	}
	template<typename... T>
	inline constexpr auto TypeName_v = Detail::NameOf<T...>();

	/************************************************************************************/
	// IsLambda_v

	namespace Detail {
		template<typename T>
		constexpr bool IsLambda() {
			constexpr std::string_view tn = TypeName_v<T>;
#if defined(__clang__)
			// lambda 输出长相: (lambda at /full_path/fileName.ext:line:colum)
			return tn.rfind("(lambda at ", 0) == 0 && *tn.rbegin() == ')' && tn.find(':') > tn.rfind('(');
#elif defined(__GNUC__)
			// lambda 输出长相: ... <lambda( ... )>
			if constexpr (tn.size() < 10 || tn[tn.size() - 1] != '>' || tn[tn.size() - 2] != ')') return false;
			auto at = tn.size() - 3;
			for (std::size_t i = 1; i; --at) {
				i += tn[at = tn.find_last_of("()", at)] == '(' ? -1 : 1;
			}
			return at >= 6 && tn.substr(at - 6, 7) == "<lambda";
#elif defined(_MSC_VER) && _MSC_VER >= 1920 && _MSC_VER < 1930  // vs2019 16.0
			// lambda 输出长相: class ????::<lambda_????>
			return tn.rfind("class ", 0) == 0 && tn.find("::<lambda_", 0) != tn.npos && *tn.rbegin() == '>';
#endif
		}
	}

	template<typename T>
	struct IsLambda : std::integral_constant<bool, Detail::IsLambda<T>()> {
	};

	template<typename T>
	inline constexpr bool IsLambda_v = IsLambda<T>::value;





	/************************************************************************************/
	// Scope Guard( F == lambda )

	template<class F>
	auto MakeScopeGuard(F&& f) noexcept {
		struct ScopeGuard {
			F f;
			bool cancel;

			explicit ScopeGuard(F&& f) noexcept : f(std::move(f)), cancel(false) {}

			~ScopeGuard() noexcept { if (!cancel) { f(); } }

			inline void Cancel() noexcept { cancel = true; }

			inline void operator()(bool cancel = false) {
				f();
				this->cancel = cancel;
			}
		};
		return ScopeGuard(std::forward<F>(f));
	}



	/************************************************************************************/
	// time_point <--> .net DateTime.Now.ToUniversalTime().Ticks converts

	// 经历时间精度: 秒后 7 个 0( 这是 windows 下最高精度. android/ios 会低1个0的精度 )
	typedef std::chrono::duration<long long, std::ratio<1LL, 10000000LL>> duration_10m;

	// 时间点 转 epoch (精度为秒后 7 个 0)
	inline int64_t TimePointToEpoch10m(std::chrono::system_clock::time_point const& val) noexcept {
		return std::chrono::duration_cast<duration_10m>(val.time_since_epoch()).count();
	}
	inline int64_t TimePointToEpoch10m(std::chrono::steady_clock::time_point const& val) noexcept {
		return std::chrono::duration_cast<duration_10m>(val.time_since_epoch()).count();
	}

	//  epoch (精度为秒后 7 个 0) 转 时间点
	inline std::chrono::system_clock::time_point Epoch10mToTimePoint(int64_t const& val) noexcept {
		return std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(duration_10m(val)));
	}


	// 得到当前时间点
	inline std::chrono::system_clock::time_point Now() noexcept {
		return std::chrono::system_clock::now();
	}
	inline std::chrono::system_clock::time_point NowTimePoint() noexcept {
		return std::chrono::system_clock::now();
	}
	inline std::chrono::steady_clock::time_point NowSteadyTimePoint() noexcept {
		return std::chrono::steady_clock::now();
	}

	// 得到当前时间点的 epoch ticks(精度为秒后 7 个 0)
	inline int64_t NowEpoch10m() noexcept {
		return TimePointToEpoch10m(NowTimePoint());
	}

	// 得到当前时间点的 epoch 微妙
	inline int64_t NowEpochMicroseconds() noexcept {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	// 得到当前时间点的 epoch 毫秒
	inline int64_t NowEpochMilliseconds() noexcept {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	// 得到当前时间点的 epoch 秒
	inline double NowEpochSeconds() noexcept {
		return (double)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000000.0;
	}
	// 得到当前时间点的 epoch 秒 更新并返回和 last 的时间差. last 可用 NowEpochSeconds 初始化
	inline double NowEpochSeconds(double& last) noexcept {
		auto now = NowEpochSeconds();
		auto rtv = now - last;
		last = now;
		return rtv;
	}

	// epoch (精度为秒后 7 个 0) 转为 .Net DateTime Utc Ticks
	inline int64_t Epoch10mToUtcDateTimeTicks(int64_t const& val) noexcept {
		return val + 621355968000000000LL;
	}

	// .Net DateTime Utc Ticks 转为 epoch (精度为秒后 7 个 0)
	inline int64_t UtcDateTimeTicksToEpoch10m(int64_t const& val) noexcept {
		return val - 621355968000000000LL;
	}

	// 时间点 转 epoch (精度为秒)
	inline int32_t TimePointToEpoch(std::chrono::system_clock::time_point const& val) noexcept {
		return (int32_t)(val.time_since_epoch().count() / 10000000);
	}

	//  epoch (精度为秒) 转 时间点
	inline std::chrono::system_clock::time_point EpochToTimePoint(int32_t const& val) noexcept {
		return std::chrono::system_clock::time_point(std::chrono::system_clock::time_point::duration((int64_t)val * 10000000));
	}

	// 得到当前时间点的 epoch ticks(精度为秒后 7 个 0)
	inline int64_t NowSteadyEpoch10m() noexcept {
		return TimePointToEpoch10m(NowSteadyTimePoint());
	}

	// 得到当前时间点的 epoch 微妙
	inline int64_t NowSteadyEpochMicroseconds() noexcept {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

	// 得到当前时间点的 epoch 毫秒
	inline int64_t NowSteadyEpochMilliseconds() noexcept {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

	// 得到当前时间点的 epoch 秒
	inline double NowSteadyEpochSeconds() noexcept {
		return (double)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() / 1000000.0;
	}
	// 得到当前时间点的 epoch 秒 更新并返回和 last 的时间差. last 可用 NowEpochSeconds 初始化
	inline double NowSteadyEpochSeconds(double& last) noexcept {
		auto now = NowSteadyEpochSeconds();
		auto rtv = now - last;
		last = now;
		return rtv;
	}


	//	// 当前时间转为字符串并填充
	//	inline void NowToString(std::string& s) noexcept {
	//		auto&& t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//		std::tm tm;
	//#ifdef _WIN32
	//		localtime_s(&tm, &t);
	//#else
	//		localtime_r(&t, &tm);
	//#endif
	//		std::stringstream ss;
	//		ss << std::put_time(&tm, "%Y-%m-%d %X");
	//		s += ss.str();
	//	}
	//
	//	// 当前时间转为字符串并返回
	//	inline std::string NowToString() noexcept {
	//		std::string s;
	//		NowToString(s);
	//		return s;
	//	}





//	/************************************************************************************/
//	// StructMemberCount 统计结构体成员变量个数
//
//	namespace Detail {
//		template<std::size_t N> struct PriorityTag : PriorityTag<N - 1> {};
//		template<> struct PriorityTag<0> {};
//
//		template<class T> struct AnyConverter {
//			template <class U, class = std::enable_if_t<!std::is_same<typename std::decay<T>::type, typename std::decay<U>::type>::value>>
//			constexpr operator U ();
//		};
//		template<class T, std::size_t I> struct AnyConverterTag : AnyConverter<T> {};
//
//		template<class T, class...Args> constexpr auto IsAggregateConstructibleImpl(T&&, Args &&...args) -> decltype(T{ std::forward<Args>(args)... }, std::true_type());
//		template<class T, class Arg> constexpr auto IsAggregateConstructibleImpl(T&&, Arg&& args) -> decltype(T{ std::forward<Arg>(args) }, std::true_type());
//		constexpr auto IsAggregateConstructibleImpl(...)->std::false_type;
//		template<class T, class...Args> struct IsAggregateConstructible : decltype(IsAggregateConstructibleImpl(std::declval<T>(), std::declval<Args>()...)) {};
//
//		template<class T, std::size_t...I> constexpr auto StructMemberCountImpl(std::index_sequence<I...>, PriorityTag<2>) -> std::enable_if_t<IsAggregateConstructible<T, AnyConverterTag<T, I>...>::value, std::size_t> { return sizeof...(I); }
//		constexpr auto StructMemberCountImpl(std::index_sequence<>, PriorityTag<1>) -> std::size_t { return 0; }
//		template<class T, class Seq> constexpr auto StructMemberCountImpl(Seq, PriorityTag<0>) -> std::size_t { return StructMemberCountImpl<T>(std::make_index_sequence<Seq::size() - 1>(), PriorityTag<2>()); };
//		template<class T> constexpr auto StructMemberCountImpl() -> std::size_t { return StructMemberCountImpl<T>(std::make_index_sequence<sizeof(T)>(), PriorityTag<2>()); };
//	}
//
//	template<class T> struct StructMemberCount : std::integral_constant<std::size_t, Detail::StructMemberCountImpl< std::remove_cv_t<std::remove_reference_t<T>> >()> {};
//
//
//	/************************************************************************************/
//	// 利用 structured binding 以及宏展开 实现依次访问 结构体 每个成员
//
//#define ENUM_PARAMS_0(x)
//#define ENUM_PARAMS_1(x) x ## 1
//#define ENUM_PARAMS_2(x) ENUM_PARAMS_1(x), x ## 2
//#define ENUM_PARAMS_3(x) ENUM_PARAMS_2(x), x ## 3
//#define ENUM_PARAMS_4(x) ENUM_PARAMS_3(x), x ## 4
//#define ENUM_PARAMS_5(x) ENUM_PARAMS_4(x), x ## 5
//#define ENUM_PARAMS_6(x) ENUM_PARAMS_5(x), x ## 6
//#define ENUM_PARAMS_7(x) ENUM_PARAMS_6(x), x ## 7
//#define ENUM_PARAMS_8(x) ENUM_PARAMS_7(x), x ## 8
//#define ENUM_PARAMS_9(x) ENUM_PARAMS_8(x), x ## 9
//#define ENUM_PARAMS_10(x) ENUM_PARAMS_9(x), x ## 10
//#define ENUM_PARAMS_11(x) ENUM_PARAMS_10(x), x ## 11
//#define ENUM_PARAMS_12(x) ENUM_PARAMS_11(x), x ## 12
//#define ENUM_PARAMS_13(x) ENUM_PARAMS_12(x), x ## 13
//#define ENUM_PARAMS_14(x) ENUM_PARAMS_13(x), x ## 14
//#define ENUM_PARAMS_15(x) ENUM_PARAMS_14(x), x ## 15
//#define ENUM_PARAMS_16(x) ENUM_PARAMS_15(x), x ## 16
//#define ENUM_PARAMS(x, N) ENUM_PARAMS_##N(x)
//
//#define ENUM_FOR_EACH_0(x)
//#define ENUM_FOR_EACH_1(x) ENUM_FOR_EACH_0(x) x(1)
//#define ENUM_FOR_EACH_2(x) ENUM_FOR_EACH_1(x) x(2)
//#define ENUM_FOR_EACH_3(x) ENUM_FOR_EACH_2(x) x(3)
//#define ENUM_FOR_EACH_4(x) ENUM_FOR_EACH_3(x) x(4)
//#define ENUM_FOR_EACH_5(x) ENUM_FOR_EACH_4(x) x(5)
//#define ENUM_FOR_EACH_6(x) ENUM_FOR_EACH_5(x) x(6)
//#define ENUM_FOR_EACH_7(x) ENUM_FOR_EACH_6(x) x(7)
//#define ENUM_FOR_EACH_8(x) ENUM_FOR_EACH_7(x) x(8)
//#define ENUM_FOR_EACH_9(x) ENUM_FOR_EACH_8(x) x(9)
//#define ENUM_FOR_EACH_10(x) ENUM_FOR_EACH_9(x) x(10)
//#define ENUM_FOR_EACH_11(x) ENUM_FOR_EACH_10(x) x(11)
//#define ENUM_FOR_EACH_12(x) ENUM_FOR_EACH_11(x) x(12)
//#define ENUM_FOR_EACH_13(x) ENUM_FOR_EACH_12(x) x(13)
//#define ENUM_FOR_EACH_14(x) ENUM_FOR_EACH_13(x) x(14)
//#define ENUM_FOR_EACH_15(x) ENUM_FOR_EACH_14(x) x(15)
//#define ENUM_FOR_EACH_16(x) ENUM_FOR_EACH_15(x) x(16)
//#define ENUM_FOR_EACH(x, N) ENUM_FOR_EACH_##N(x)
//
//	namespace Detail {
//#define APPLYER_DEF(N) \
//    template<class T, class ApplyFunc> auto StructApply_impl(T &&in, ApplyFunc &&f, std::integral_constant<std::size_t, N>) { \
//        auto &&[ENUM_PARAMS(x, N)] = std::forward<T>(in); \
//        return f(ENUM_PARAMS(x, N)); \
//    }; \
//
//		ENUM_FOR_EACH(APPLYER_DEF, 16)
//#undef APPLYER_DEF
//
//			template<class T, class ApplyFunc> auto StructApply(T&& in, ApplyFunc&& f) {
//			return StructApply_impl(std::forward<T>(in), std::forward<ApplyFunc>(f), xx::StructMemberCount<T>());
//		};
//	}
//
//	template<class T> auto StructToTuple(T&& in) {
//		return Detail::StructApply(std::forward<T>(in), [](auto const&...args) { return std::make_tuple(args...); });
//	};
//
//	template<class T, class U> void TupleToStruct(T& out, U&& in) {
//		Detail::StructApply(out, [in = std::forward<U>(in)](auto&...args) { return std::tie(args...) = in; });
//	};
//
//	//auto&& t1 = StructToTuple(A{ 1, '2', 3.0 }); // std::tuple<int, char, double> a{1, '2', 3.0}
//	//std::cout << std::get<0>(t1) << std::get<1>(t1) << std::get<2>(t1) << std::endl;
//	//A a2; TupleToStruct(a2, std::make_tuple(4, '5', 6.0)); // a = A{4, '5', 6.0};
//	//std::cout << a2.v1 << a2.v2 << a2.v3 << std::endl;
//
//	template<class T> auto StructToPtrTuple(T&& in) {
//		return Detail::StructApply(std::forward<T>(in), [](auto&&...args) { return std::make_tuple(&args...); });
//	};
//
//
//
//	// for 打印结构体数据
//	struct TupleDumper {
//
//		template<std::size_t I = 0, typename... Tp>
//		static std::enable_if_t<I == sizeof...(Tp) - 1> DumpTuple(std::tuple<Tp...> const& t) {
//			Dump(std::get<I>(t));
//		}
//
//		template<std::size_t I = 0, typename... Tp>
//		static std::enable_if_t < I < sizeof...(Tp) - 1> DumpTuple(std::tuple<Tp...> const& t) {
//			Dump(std::get<I>(t));
//			DumpTuple<I + 1, Tp...>(t);
//		}
//
//		template<typename T>
//		static void Dump(T const* const& v) {
//			if constexpr (xx::IsTuple_v<T>) {
//				DumpTuple(*v);
//			}
//			else if constexpr (std::is_same_v<T, std::string>) {
//				std::cout << *v << " ";
//			}
//			else if constexpr (std::is_class_v<T>) {
//				auto t = xx::StructToPtrTuple(*v);
//				DumpTuple(t);
//			}
//			else {
//				std::cout << *v << " ";
//			}
//		}
//	};
//
//	// 打印结构体数据
//	template<class T>
//	void DumpStruct(T const& in) {
//		TupleDumper::DumpTuple(StructToPtrTuple(in));
//	}
}
