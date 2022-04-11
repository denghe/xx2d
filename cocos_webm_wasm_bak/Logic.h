#pragma once
#ifdef EMSCRIPTEN
#define EMSCRIPTEN_KEEPALIVE __attribute__((used)) __attribute__((visibility("default")))
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
	void* EMSCRIPTEN_KEEPALIVE LogicNew();
	void EMSCRIPTEN_KEEPALIVE LogicDelete(void* logic);
	void* EMSCRIPTEN_KEEPALIVE LogicGetInBuf(void* logic);
	void* EMSCRIPTEN_KEEPALIVE LogicGetOutBuf(void* logic);

	int EMSCRIPTEN_KEEPALIVE LogicTouchBegan(void* logic, int ki, float x, float y);
	void EMSCRIPTEN_KEEPALIVE LogicTouchMoved(void* logic, int ki, float x, float y);
	void EMSCRIPTEN_KEEPALIVE LogicTouchEnded(void* logic, int ki, float x, float y);
	void EMSCRIPTEN_KEEPALIVE LogicTouchCancelled(void* logic, int ki);

	void EMSCRIPTEN_KEEPALIVE LogicUpdate(void* logic, float delta);

	// todo: network ...
}
