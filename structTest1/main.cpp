// 简单模拟 godot 的场景组织结构和事件
#include <thread>
#include <vector>
#include <chrono>
#include "xx_ptr.h"	// xx::Shared xx::Weak

struct Env;
struct SceneTree;
struct Viewport;
struct Node;

struct SceneTree {
	xx::Shared<Viewport> root;

	xx::Shared<Viewport>& GetRoot() { return root; }
};

struct Node {
	virtual ~Node() = default;

	xx::Weak<SceneTree> tree;
	xx::Weak<Node> parent;
	std::vector<xx::Shared<Node>> children;

	std::string name;

	virtual void EnterTree() {}
	virtual void ExitTree() {}
	virtual void Ready() {}

	void CallEnterTree();
	void CallExitTree();
	void CallReady();

	xx::Shared<SceneTree> GetTree();
	void AddChild(xx::Shared<Node> const& node);
	void RemoveChild(xx::Shared<Node> const& node);
	// todo: GetNode("/root")
	// todo: print_tree_pretty
};

struct Viewport : Node {
	Viewport() {
		name = "root";
	}
};

xx::Shared<SceneTree> Node::GetTree() {
	return tree.Lock();
}
void Node::AddChild(xx::Shared<Node> const& node) {
	children.push_back(node);
	node->CallEnterTree();
}
void Node::RemoveChild(xx::Shared<Node> const& node) {
	auto iter = std::find(children.begin(), children.end(), node);
	if (iter != children.end()) {
		node->CallExitTree();
		children.erase(iter);
	}
}
void Node::CallEnterTree() {
	assert(tree);
	EnterTree();
	for (auto&& c : children) {
		c->CallEnterTree();
	}
}
void Node::CallExitTree() {
	assert(tree);
	for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
		(*iter)->CallExitTree();
		(*iter)->tree.Reset();
	}
	ExitTree();
	tree.Reset();
}
void Node::CallReady() {
	assert(tree);
	for (auto&& c : children) {
		c->CallReady();
	}
	Ready();
}

struct Env {
	xx::Shared<SceneTree> tree;

	Env() {
		tree.Emplace();
	}

	int MainLoop() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(16));

		}
		return 0;
	}
};

int main() {
	Env env;
	return env.MainLoop();
}
