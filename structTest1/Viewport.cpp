#include "Viewport.h"
#include "SceneTree.h"

Viewport::Viewport(SceneTree* tree) : Node(tree) {
	name = "root";
	entered = true;
}
