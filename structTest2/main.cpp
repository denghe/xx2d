#include "xx_typehelpers.h"

//#define XX_STRINGIFY_LIST(...) #__VA_ARGS__
//#define XX_REGISTER(TYPE, ...) struct TYPE { __VA_ARGS__; XX_STRINGIFY_LIST(__VA_ARGS__); }
//
//#define PRIVATE_ARGS_GLUE(x, y) x y
//#define PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
//#define PRIVATE_MACRO_VAR_ARGS_IMPL(args)  PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT args
//#define COUNT_MACRO_VAR_ARGS(...)  PRIVATE_MACRO_VAR_ARGS_IMPL((__VA_ARGS__, 10, 9, 8, 7, 6, 5 4, 3, 2, 1, 0))
//
//#define PRIVATE_MACRO_CHOOSE_HELPER2(M, count)  M##count
//#define PRIVATE_MACRO_CHOOSE_HELPER1(M, count)  PRIVATE_MACRO_CHOOSE_HELPER2(M, count)
//#define PRIVATE_MACRO_CHOOSE_HELPER(M, count)   PRIVATE_MACRO_CHOOSE_HELPER1(M, count)
//
//#define INVOKE_VAR_MACRO(M, ...) PRIVATE_ARGS_GLUE(PRIVATE_MACRO_CHOOSE_HELPER(M, COUNT_MACRO_VAR_ARGS(__VA_ARGS__)), (__VA_ARGS__))
//
//
//#define PRINT_ARGS1(a1) std::cout << a1
//#define PRINT_ARGS2(a1, a2) PRINT_ARGS1(a1) << "," << a2
//#define PRINT_ARGS3(a1, a2, a3) PRINT_ARGS2(a1, a2) << "," << a3
//#define PRINT_ARGS4(a1, a2, a3, a4) PRINT_ARGS3(a1, a2, a3) << "," << a4
//#define PRINT_ARGS5(a1, a2, a3, a4, a5) PRINT_ARGS4(a1, a2, a3, a4) << "," << a5
//#define PRINT_ARGS6(a1, a2, a3, a4, a5, a6) PRINT_ARGS5(a1, a2, a3, a4, a5) << "," << a6
//
//
//#define XX_REGISTER1(T) Register<T>(XX_STRINGIFY(T))
//#define XX_REGISTER2(T, F1)											XX_REGISTER1(T).Field(XX_STRINGIFY(F1), &T::F1)
//#define XX_REGISTER3(T, F1, F2)										XX_REGISTER2(T, F1).Field(XX_STRINGIFY(F2), &T::F2)
//#define XX_REGISTER4(T, F1, F2, F3)									XX_REGISTER3(T, F1, F2).Field(XX_STRINGIFY(F3), &T::F3)
//#define XX_REGISTER5(T, F1, F2, F3, F4)								XX_REGISTER4(T, F1, F2, F3).Field(XX_STRINGIFY(F4), &T::F4)
//#define XX_REGISTER6(T, F1, F2, F3, F4, F5)							XX_REGISTER5(T, F1, F2, F3, F4).Field(XX_STRINGIFY(F5), &T::F5)
//#define XX_REGISTER7(T, F1, F2, F3, F4, F5, F6)						XX_REGISTER6(T, F1, F2, F3, F4, F5).Field(XX_STRINGIFY(F6), &T::F6)
//
//#define XX_REGISTER_CLASS(...) INVOKE_VAR_MACRO(XX_REGISTER, __VA_ARGS__)


// https://stackoverflow.com/questions/45756920/c-c-preprocessor-extract-every-second-variadic-parameter

#define EVERY_SECOND0(...)

#define EVERY_SECOND1_(second, ...) , second
#define EVERY_SECOND1(first, ...) EVERY_SECOND1_(__VA_ARGS__)

#define EVERY_SECOND2_(second, ...) , second EVERY_SECOND1(__VA_ARGS__)
#define EVERY_SECOND2(first, ...) EVERY_SECOND2_(__VA_ARGS__)

#define EVERY_SECOND3_(second, ...) , second EVERY_SECOND2(__VA_ARGS__)
#define EVERY_SECOND3(first, ...) EVERY_SECOND3_(__VA_ARGS__)

#define EVERY_SECOND4_(second, ...) , second EVERY_SECOND3(__VA_ARGS__)
#define EVERY_SECOND4(first, ...) EVERY_SECOND4_(__VA_ARGS__)

#define EVERY_SECOND5_(second, ...) , second EVERY_SECOND4(__VA_ARGS__)
#define EVERY_SECOND5(first, ...) EVERY_SECOND5_(__VA_ARGS__)

#define COUNT_EVERY_SECOND(_1,__1,_2,__2,_3,__3,_4,__4,_5,__5,num,...) EVERY_SECOND ## num
#define EVERY_SECOND(...) COUNT_EVERY_SECOND(__VA_ARGS__,5,5,4,4,3,3,2,2,1,0)(__VA_ARGS__)


#define INTERFACE_FN(T, ...) Register<T>(XX_STRINGIFY(T)) EVERY_SECOND(__VA_ARGS__));

template<typename T>
struct Register {
	template<typename Str>
	Register(Str&& typeName) {}

	template<typename F, typename Str>
	Register& Field(Str&& fieldName, F&& fieldPtr) { return *this; }
};

struct Foo {
	int a, b, c;
};



int main() {

	//INTERFACE_FN(Foo, a, b, c);

	//INVOKE_VAR_MACRO(PRINT_ARGS, 4, 3, 5, 1);

	//XX_REGISTER_CLASS(Foo);
	//XX_REGISTER_CLASS(Foo, a);
	//XX_REGISTER_CLASS(Foo, a, b);
	//XX_REGISTER_CLASS(Foo, a, b, c);

	return 0;
}
