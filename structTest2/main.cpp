#include "xx_typehelpers.h"

#include "xx_data.h"


struct A {
	int a1 = 0;
	std::string a2;
};
struct B {
	A a3;
	double a4 = 0;
};
struct C : B {
	float a5 = 0;
};

int main() {
	auto [a1, a2, a3] = C{ 1, "asdf", 1.2f };
	std::cout << a1 << std::endl;
	std::cout << a2.a1 << std::endl;
	std::cout << a2.a2 << std::endl;
	std::cout << a3 << std::endl;

	//C b{ { 1, "2asdf" }, 3.0, 1.0f };
	//xx::DumpStruct(b);
	//std::cout << std::endl;

	//xx::Data d;
	//d.WriteStruct(b);
	//xx::DumpStruct(d);
	//std::cout << std::endl;

	//B c;
	//int r = d.ReadStruct(c);
	//assert(r == 0);
	//xx::DumpStruct(c);
	//std::cout << std::endl;
}
