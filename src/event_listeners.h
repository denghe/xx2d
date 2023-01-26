#pragma once
#include "pch.h"

/*

example:

struct Foo {
	bool HandleMouseDown(MouseEventListener<Foo>* L);
	int HandleMouseMove(MouseEventListener<Foo>* L);
	void HandleMouseUp(MouseEventListener<Foo>* L);
};

*/

template<typename Handler>
struct MouseEventListener {
	Engine* eg{};
	Mbtns btn{};

	XY downPos{}, lastPos{};
	double downTime{};

	uint8_t lastState{};
	Handler* handler{};
	int eventId{};	// 0: no event   1: down  2: move  3: up  4: cancel?

	// todo: helper funcs?

	void Init(Engine* eg, Mbtns btn) {
		this->eg = eg;
		this->btn = btn;
	}

	// eventId > 0: need Dispatch
	void Update() {
		if (auto&& state = eg->mbtnStates[(size_t)btn]; lastState != state) {
			lastState = state;
			if (state) {	// down
				assert(!handler);
				lastPos = downPos = eg->mousePosition;
				downTime = eg->lastTime;
				eventId = 1;	// need search handler
			} else {	// up
				if (handler) {
					handler->HandleMouseUp(this);
					handler = {};
					eventId = {};
				}
			}
		} else {
			if (handler && lastPos != eg->mousePosition) {	// move
				eventId = handler->HandleMouseMove(this);	// 1? search next handler?
				lastPos = eg->mousePosition;
			}
		}
	}

	// eventId > 0: need Dispatch next handler
	void Dispatch(Handler* h) {
		assert(!handler);
		if (h->HandleMouseDown(this)) {
			handler = h;
			eventId = {};
		}
	}
};
