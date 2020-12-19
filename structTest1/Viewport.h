#pragma once
#include "Node.h"

struct Viewport : Node {
	Viewport(SceneTree* const& tree);
	Viewport(Viewport const&) = delete;
	Viewport& operator=(Viewport const&) = delete;
};
