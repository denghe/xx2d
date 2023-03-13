#pragma once
#include "xx2d.h"

struct GameLooper;
struct SceneBase {
	GameLooper* looper{};
	virtual void Init(GameLooper*) = 0;
	virtual int Update() = 0;
	virtual ~SceneBase() {};
};
