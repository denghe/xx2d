#pragma once
#include "pch.h"

struct Logic : Engine {
	xx::Coros coros;
	void Init();
	int Update();
};
