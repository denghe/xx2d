#include "xx_typehelpers.h"

template<typename T>
struct Register {
	template<typename Str>
	Register(Str&& typeName) {}

	template<typename F, typename Str>
	Register& Field(Str&& fieldName, F&& fieldPtr) { return *this; }
};

struct Foo {
	int a, b, c, d;
};


#define _my_BUGFX(x) x

#define _my_NARG2(...) _my_BUGFX(_my_NARG1(__VA_ARGS__,_my_RSEQN()))
#define _my_NARG1(...) _my_BUGFX(_my_ARGSN(__VA_ARGS__))
#define _my_ARGSN(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N
#define _my_RSEQN() 10,9,8,7,6,5,4,3,2,1,0

#define _my_FUNC2(name,n) name ## n
#define _my_FUNC1(name,n) _my_FUNC2(name,n)
#define GET_MACRO(func,...) _my_FUNC1(func,_my_BUGFX(_my_NARG2(__VA_ARGS__))) (__VA_ARGS__)




// prints a single offset
#define PRN_STRUCT_OFFSET(x, a) printf("&" #x "." #a " = %d\n", offsetof(x, a));

// prints a struct with one member
#define PRN_STRUCT_OFFSETS_2(x, a) PRN_STRUCT_OFFSET(x, a)

// prints a struct with two members
#define PRN_STRUCT_OFFSETS_3(x, a, b) \
            PRN_STRUCT_OFFSET(x, a) \
            PRN_STRUCT_OFFSET(x, b)

// prints a struct with two members
#define PRN_STRUCT_OFFSETS_4(x, a, b, c) \
            PRN_STRUCT_OFFSET(x, a) \
            PRN_STRUCT_OFFSET(x, b) \
            PRN_STRUCT_OFFSET(x, c)


#define STRINGIFY_LIST(...) #__VA_ARGS__


template<typename T>
struct TypeMembersName;

template<typename T, size_t I>
static constexpr std::string_view GetMemberName() {
	size_t a = 0, b = 0, i = 0;
	auto&& mns = TypeMembersName<T>::memberNames;
	for (; i < mns.size(); ++i) {
		if (mns[i] == ',') {
			if (a < I) {
				++a;
				b = i + 1;
				if (mns[b] == ' ') ++b;
			}
			else break;
		}
	}
	return std::string_view(&mns[b], i - b);
}

template<typename T, typename...FS>
void RegisterFields(T* self, FS&...fs) {
	// todo: fs 依次取地址 并减去 self 从而得到 offset ?
}


// 下面这些代码放到宏
template<>
struct TypeMembersName<Foo> {
	static constexpr std::string_view memberNames = "a, b, c";	// STRINGIFY_LIST(__VA_ARGS__)
	//inline static std::map<std::string_view, size_t> memberOffsets;
	struct Register_Foo : Foo {
		Register_Foo() {
			RegisterFields<Foo>(this, a, b, c);	// __VA_ARGS__
		}
	};
	inline static Register_Foo instance_Register_Foo;
};


int main() {

	std::cout << GetMemberName<Foo, 0>() << std::endl;
	std::cout << GetMemberName<Foo, 1>() << std::endl;
	std::cout << GetMemberName<Foo, 2>() << std::endl;

	//PRN_STRUCT_OFFSETS_4(Foo, a, b, c);
	//STRINGIFY_LIST(a, b, c);
	// todo: 上面这个宏如果包一层, 将无法展开参数
	// 思路2 将 STRINGIFY_LIST 转为 string_view 然后切割?

	return 0;
}
