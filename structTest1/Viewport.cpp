#include "Viewport.h"
#include "SceneTree.h"

Viewport::Viewport(SceneTree* const& tree) : Node(tree) {
	name = "root";
	entered = true;
}
