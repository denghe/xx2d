#include "pch.h"
#include "logic.h"
#include "logic8.h"

void Logic8::Init(Logic* logic) {
	this->logic = logic;

	std::cout << "Logic8 Init( matrix tests )" << std::endl;

	xx::Node n;
	n.radians = M_PI / 2;
	n.transform = xx::AffineTransform::Make(n.pos, n.anchorSize, n.scale, n.radians);

	auto xy = n.transform.Apply({ 5, 0 });
	std::cout << xy.x << std::endl;
	std::cout << xy.y << std::endl;

	//auto t1 = xx::AffineTransform::Rotate(xx::AffineTransform::MakeIdentity(), M_PI / 2.f);
	//auto xy = t1.Apply({ 5, 0 });
	//std::cout << xy.x << std::endl;
	//std::cout << xy.y << std::endl;
}

int Logic8::Update() {

	return 0;
}
