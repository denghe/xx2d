#pragma once
#include "xx_ptr.h"	// xx::Shared xx::Weak

struct SceneTree;
struct Viewport;
struct Node;

struct SceneTree {
	xx::Shared<Viewport> root;
	xx::Shared<Viewport>& GetRoot() { return root; }

	bool removeProtect = false;

	SceneTree();
	SceneTree(SceneTree const&) = delete;
	SceneTree& operator=(SceneTree const&) = delete;

	int MainLoop();

	template<typename T, typename ...Args, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> CreateNode(Args&&...args) {
		return xx::MakeShared<T>(this, std::forward<Args>(args)...);
	}
};
