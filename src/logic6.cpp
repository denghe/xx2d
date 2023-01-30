#include "pch.h"
#include "logic.h"
#include "logic6.h"

void Logic6::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic6 Init( test box + circle collision detect )" << std::endl;

	cs.emplace_back().Init({}, 100, 64);
	bs.emplace_back().Init({}, {50, 200});

	BL.Init(eg, Mbtns::Left);
	CL.Init(eg, Mbtns::Right);
}

int Logic6::Update() {

	{
		CL.Update();
		auto&& iter = cs.begin();
		while (CL.eventId && iter != cs.end()) {
			CL.Dispatch(&*iter++);
		}
	}
	{
		BL.Update();
		auto&& iter = bs.begin();
		while (BL.eventId && iter != bs.end()) {
			BL.Dispatch(&*iter++);
		}
	}

	for (auto& c : cs) {
		c.border.Draw(eg);
	}
	for (auto& b : bs) {
		b.border.Draw(eg);
	}

	return 0;
}
