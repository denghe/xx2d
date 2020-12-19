#include "SceneTree.h"
#include "Viewport.h"
#include <thread>

SceneTree::SceneTree() {
	root.Emplace(this);
}

int SceneTree::MainLoop() {
	while (true) {
		// simulate frame delay
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		float delta = 0.016f;
		root->CallProcess(delta);
	}
	return 0;
}
