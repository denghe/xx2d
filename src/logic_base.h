#pragma once
#include "pch.h"

struct Logic;
struct LogicBase {
	Logic* logic{};
	virtual void Init(Logic*) = 0;
	virtual int Update() = 0;
	virtual ~LogicBase() {};
};
