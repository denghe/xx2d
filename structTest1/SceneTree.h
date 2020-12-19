#pragma once
#include "xx_ptr.h"	// xx::Shared xx::Weak

struct SceneTree;
struct Viewport;
struct Node;

struct SceneTree {
	SceneTree();
	SceneTree(SceneTree const&) = delete;
	SceneTree& operator=(SceneTree const&) = delete;

	bool removeProtect = false;
	double beginSeconds = 0;
	double lastSeconds = 0;
	xx::Shared<Viewport> root;

	int MainLoop();

	xx::Shared<Viewport>& GetRoot() { return root; }

	template<typename T, typename ...Args, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> CreateNode(Args&&...args) {
		return xx::MakeShared<T>(this, std::forward<Args>(args)...);
	}
};
