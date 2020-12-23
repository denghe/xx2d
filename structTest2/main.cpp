#include "xx_typehelpers.h"

#include "xx_data.h"


struct A { int v1; std::string v2; };
struct B { A a; double v3; };

int main() {
	B b{ { 1, "2asdf" }, 3.0 };
	xx::DumpStruct(b);
	std::cout << std::endl;

	xx::Data d;
	d.WriteStruct(b);
	xx::DumpStruct(d);
	std::cout << std::endl;

	B c;
	int r = d.ReadStruct(c);
	assert(r == 0);
	xx::DumpStruct(c);
	std::cout << std::endl;
}
