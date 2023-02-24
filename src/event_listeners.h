#pragma once
#include "pch.h"

/*
example:

struct Foo;
using FooMouseEventListener = MouseEventListener<Foo*>;

struct Foo {
	bool HandleMouseDown(FooMouseEventListener& L);
	int HandleMouseMove(FooMouseEventListener& L);
	void HandleMouseUp(FooMouseEventListener& L);
};
...
FooMouseEventListener meListener;
...
meListener.Init(this, Mbtns::Left);
...
meListener.Update();
auto&& iter = foos.begin();
while (meListener.eventId && iter != foos.end()) {
	meListener.Dispatch(&*iter++);
}

*/

namespace xx {

	template<typename Handler>
	struct MouseEventListener {
		Mbtns btn{};

		XY downPos{}, lastPos{};
		double downTime{};

		uint8_t lastState{};
		Handler handler{};
		int eventId{};	// 0: no event   1: down  2: move  3: up  4: cancel?

		// todo: helper funcs?

		void Init(Mbtns btn) {
			this->btn = btn;
		}

		// eventId > 0: need Dispatch
		void Update() {
			if (auto&& state = engine.mbtnStates[(size_t)btn]; lastState != state) {
				lastState = state;
				if (state) {	// down
					assert(!handler);
					lastPos = downPos = engine.mousePosition;
					downTime = engine.nowSecs;
					eventId = 1;	// need search handler
				} else {	// up
					if (handler) {
						handler->HandleMouseUp(*this);
						handler = {};
						eventId = {};
					}
				}
			} else {
				if (handler && lastPos != engine.mousePosition) {	// move
					if ((eventId = handler->HandleMouseMove(*this))) {
						handler = {};
					} else {
						lastPos = engine.mousePosition;
					}
				}
			}
		}

		// eventId > 0: need Dispatch next handler
		template<typename H>
		void Dispatch(H&& h) {
			assert(!handler);
			if (h->HandleMouseDown(*this)) {
				handler = std::forward<H>(h);
				eventId = {};
			}
		}
	};

}
