#pragma once
#include "Node.h"

struct Viewport : Node {
	Viewport(SceneTree* tree);
	Viewport(Viewport const&) = delete;
	Viewport& operator=(Viewport const&) = delete;
};
