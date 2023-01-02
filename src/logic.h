#pragma once
#include "pch.h"
#include "tmx.h"

struct Logic : Engine {
	TMX::Map m;
	void Init();
	int Update();
};
