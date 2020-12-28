#include "xx_typehelpers.h"

template<typename T>
struct Register {
	Register(std::string_view const& typeName) {
		std::cout << typeName << std::endl;
	}

	template<typename F>
	Register& Field(std::string_view const& fieldName, F&& fieldPtr) {
		std::cout << fieldName << std::endl;
		return *this;
	}
};

#define REGISTER_PREFIX_(t) Register<t>(XX_STRINGIFY(t))
#define REGISTER_SUFFIX_(t, a) .Field(XX_STRINGIFY(a), &t::a)
#define REGISTER_CLASS_FIELDS(...) XX_CONCAT(XX_, XX_BUGFIX(XX_NARG(__VA_ARGS__))) \
(REGISTER_PREFIX_, REGISTER_SUFFIX_, __VA_ARGS__)

struct Foo {
	int a, b, c, d, e, f, g, h, i, j;
};

int main() {
	REGISTER_CLASS_FIELDS(Foo, a, b, c, d, e, f, g, h, i, j);
	Register<Foo>("Foo")
		.Field("a", &Foo::a)
		.Field("b", &Foo::b)
		.Field("c", &Foo::b)
		.Field("d", &Foo::b)
		.Field("e", &Foo::b)
		.Field("f", &Foo::b)
		.Field("h", &Foo::h)
		.Field("i", &Foo::i)
		.Field("j", &Foo::j);

	return 0;
}








//// Create a string out of the first argument, and the rest of the arguments.
//#define TWO_STRINGS( first, ... ) #first, #__VA_ARGS__
//#define A( ... ) TWO_STRINGS(__VA_ARGS__)
//const char* c[2] = { A(1, 2) };





//template<typename T>
//struct TypeMembersName;
//
//template<typename T, size_t I>
//static constexpr std::string_view GetMemberName() {
//	size_t a = 0, b = 0, i = 0;
//	auto&& mns = TypeMembersName<T>::memberNames;
//	for (; i < mns.size(); ++i) {
//		if (mns[i] == ',') {
//			if (a < I) {
//				++a;
//				b = i + 1;
//				if (mns[b] == ' ') ++b;
//			}
//			else break;
//		}
//	}
//	return std::string_view(&mns[b], i - b);
//}
//
//template<typename T, typename...FS>
//void RegisterFields(T* self, FS&...fs) {
//	// todo: fs 依次取地址 并减去 self 从而得到 offset ?
//}
//
//
//// 下面这些代码放到宏
//template<>
//struct TypeMembersName<Foo> {
//	static constexpr std::string_view memberNames = "a, b, c";	// STRINGIFY_LIST(__VA_ARGS__)
//	//inline static std::map<std::string_view, size_t> memberOffsets;
//	struct Register_Foo : Foo {
//		Register_Foo() {
//			RegisterFields<Foo>(this, a, b, c);	// __VA_ARGS__
//		}
//	};
//	inline static Register_Foo instance_Register_Foo;
//};
