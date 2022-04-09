#pragma once

extern "C" {
	void* LogicNew();
	void LogicDelete(void* logic);
	void* LogicGetInBuf(void* logic);
	void* LogicGetOutBuf(void* logic);

	bool LogicTouchBegan(void* logic, int ki, float x, float y);
	void LogicTouchMoved(void* logic, int ki, float x, float y);
	void LogicTouchEnded(void* logic, int ki, float x, float y);
	void LogicTouchCancelled(void* logic, int ki);

	void LogicUpdate(void* logic, float delta);

	// todo: network ...
}
