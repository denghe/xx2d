#pragma once
#include "pch.h"
#include "logic1.h"
#include "logic2.h"
// ...

struct Logic : Engine {
	BMFont fnt1;
	Label lbCount;

	//Logic1 lg;
	Logic2 lg;

	void Init();
	int Update();
};
