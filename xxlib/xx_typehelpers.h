#pragma once

#include <type_traits>
#include <cstddef>
#include <optional>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>
#include <cstdint>
#include <cassert>
#include <ctime>  // std::tm
#include <iomanip>  // std::put_time
#include <array>
#include <sstream>
#include <iostream>

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

namespace xx {
    /************************************************************************************/
    // std::is_pod 的自定义扩展, 用于标识一个类可以在容器中被r memcpy | memmove

    template<typename T, typename ENABLED = void>
    struct IsPod : std::false_type {
    };
    template<typename T>
    struct IsPod<T, std::enable_if_t<std::is_standard_layout_v<T> && std::is_trivial_v<T>>> : std::true_type {
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
    struct IsOptional<std::optional<T> &> : std::true_type {
    };
    template<typename T>
    struct IsOptional<std::optional<T> const &> : std::true_type {
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
    struct IsVector<std::vector<T> &> : std::true_type {
    };
    template<typename T>
    struct IsVector<std::vector<T> const &> : std::true_type {
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
    struct IsTuple<std::tuple<T...> &> : std::true_type {
    };
    template<typename ...T>
    struct IsTuple<std::tuple<T...> const &> : std::true_type {
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
    struct IsShared<std::shared_ptr<T> &> : std::true_type {
    };
    template<typename T>
    struct IsShared<std::shared_ptr<T> const &> : std::true_type {
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
    struct IsWeak<std::weak_ptr<T> &> : std::true_type {
    };
    template<typename T>
    struct IsWeak<std::weak_ptr<T> const &> : std::true_type {
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
    struct IsUnique<std::unique_ptr<T> &> : std::true_type {
    };
    template<typename T>
    struct IsUnique<std::unique_ptr<T> const &> : std::true_type {
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
    struct IsFunction<std::function<T> &> : std::true_type {
        using FT = T;
    };
    template<typename T>
    struct IsFunction<std::function<T> const &> : std::true_type {
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
        static inline auto Convert(T &&v) {
            return &*v;
        }
    };

    template<typename T>
    struct ToPointerFuncs<T, std::enable_if_t<!IsPointerClass_v<T> && !IsWeak_v<T>>> {
        static inline std::remove_reference_t<T> *Convert(T &&v) {
            return &v;
        }
    };

    template<typename T>
    struct ToPointerFuncs<T, std::enable_if_t<IsWeak_v<T>>> {
        static inline auto Convert(T &&v) {
            return v.lock();
        }
    };

    template<typename T>
    auto ToPointer(T &&v) {
        return ToPointerFuncs<T>::Convert(std::forward<T>(v));
    }

    /************************************************************************************/
    // RefC_t 引用类型参数容器类型路由

    template<typename T>
    struct RefWrapper {
        T *p = nullptr;

        RefWrapper() = default;

        RefWrapper(T &v) : p(&v) {}

        inline operator T &() { return *p; }
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
    struct FuncTraits<Rtv (*)(Args ...)> {
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
    struct FuncTraits<Rtv (CT::*)(Args ...)> {
        using R = Rtv;
        using A = std::tuple<RefC_t<Args>...>;
        using C = CT;
    };

    template<typename Rtv, typename CT, typename... Args>
    struct FuncTraits<Rtv (CT::*)(Args ...) const> {
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
    std::shared_ptr<T> &MakeTo(std::shared_ptr<T> &v, Args &&...args) {
        v = std::make_shared<T>(std::forward<Args>(args)...);
        return v;
    }

    template<typename T, typename ...Args>
    std::shared_ptr<T> &TryMakeTo(std::shared_ptr<T> &v, Args &&...args) noexcept {
        v = TryMake<T>(std::forward<Args>(args)...);
        return v;
    }

    template<typename T, typename U>
    std::shared_ptr<T> As(std::shared_ptr<U> const &v) noexcept {
        return std::dynamic_pointer_cast<T>(v);
    }

    template<typename T, typename U>
    bool Is(std::shared_ptr<U> const &v) noexcept {
        return std::dynamic_pointer_cast<T>(v) != nullptr;
    }

    /************************************************************************************/
    // weak_ptr 系列

    template<typename T, typename U>
    std::weak_ptr<T> AsWeak(std::shared_ptr<U> const &v) noexcept {
        return std::weak_ptr<T>(As<T>(v));
    }

    template<typename T>
    std::weak_ptr<T> ToWeak(std::shared_ptr<T> const &v) noexcept {
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
    T *Malloc() {
        return (T *) malloc(sizeof(T));
    }

    template<typename T>
    T *&MallocTo(T *&v) {
        v = (T *) malloc(sizeof(T));
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
    // Scope Guard( F == lambda )

    template<class F>
    auto MakeScopeGuard(F &&f) noexcept {
        struct ScopeGuard {
            F f;
            bool cancel;

            explicit ScopeGuard(F &&f) noexcept: f(std::move(f)), cancel(false) {}

            ~ScopeGuard() noexcept { if (!cancel) { f(); }}

            inline void Cancel() noexcept { cancel = true; }

            inline void operator()(bool cancel = false) {
                f();
                this->cancel = cancel;
            }
        };
        return ScopeGuard(std::forward<F>(f));
    }
}
