#include "pch.h"
#include "logic.h"
#include "logic6.h"

void Logic6::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic6 Init( test box + circle collision detect )" << std::endl;

	cam.offset = eg->ninePoints[7];	// (0,0) at top left
}

int Logic6::Update() {

	return 0;
}
