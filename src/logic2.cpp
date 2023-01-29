#include "pch.h"
#include "logic.h"
#include "logic2.h"

void Logic2::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic2 Init( test mouse event listener )" << std::endl;

	// test mouse event listener
	meListener.Init(eg, Mbtns::Left);

	// test line strip
	for (size_t i = 0; i < 10000; i++) {
		XY v{ float(rand() % (1920 - 30)) + 15 - eg->hw, float(rand() % (1080 - 30)) + 15 - eg->hh };

		auto&& c = circles.emplace_back();
		c.Init(v, 15, 12);
	}
}

int Logic2::Update() {

	// test mouse event listener
	meListener.Update();
	auto&& iter = circles.begin();
	while (meListener.eventId && iter != circles.end()) {
		meListener.Dispatch(&*iter++);
	}

	// test line strip
	for (auto&& c : circles) {
		c.Draw(eg);
	}

	return 0;
}

bool DragableCircle::HandleMouseDown(DragableCircleMouseEventListener& L) {
	auto dx = pos.x - L.downPos.x;
	auto dy = pos.y - L.downPos.y;
	return dx * dx + dy * dy < radius_square;
}

int DragableCircle::HandleMouseMove(DragableCircleMouseEventListener& L) {
	this->SetPositon(this->pos + (L.eg->mousePosition - L.lastPos));
	this->Commit();
	return 0;
}

void DragableCircle::HandleMouseUp(DragableCircleMouseEventListener& L) {
	std::cout << "L.downPos.x = " << L.downPos.x << ", L.downPos.y = " << L.downPos.y << std::endl;
}

void DragableCircle::Init(XY const& pos, float const& radius, int const& segments) {
	this->radius_square = radius * radius;
	this->FillCirclePoints({ 0,0 }, radius, {}, segments);
	this->SetColor({ 255, 255, 0, 255 });
	this->SetPositon(pos);
	this->Commit();
}
