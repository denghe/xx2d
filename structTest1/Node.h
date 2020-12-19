#pragma once
#include "xx_ptr.h"	// xx::Shared xx::Weak

struct SceneTree;
struct Node {
	virtual ~Node() = default;
	Node(Node const&) = delete;
	Node& operator=(Node const&) = delete;

	template<typename T = Node>
	XX_FORCEINLINE xx::Weak<T> WeakFromThis() const {
		auto h = (xx::PtrHeader*)this - 1;
		return *((xx::Weak<T>*) & h);
	}
	template<typename T = Node>
	XX_FORCEINLINE xx::Shared<T> SharedFromThis() const {
		return WeakFromThis<T>().Lock();
	}

	SceneTree* const tree;
	Node(SceneTree* tree) : tree(tree) {
		if (!tree) throw std::runtime_error("first args: tree is nullptr");
	}
	SceneTree* GetTree() const;

	bool entered = false;
	xx::Weak<Node> parent;
	std::vector<xx::Shared<Node>> children;
	std::string name;

	virtual void EnterTree() {}
	virtual void ExitTree() {}
	virtual void Ready() {}
	virtual void Process(float delta) {}

	void CallEnterTree();
	void CallExitTree();
	void CallReady();
	void CallProcess(float delta);

	void PrintTreePretty(std::string const& prefix = "", bool const& last = true) const;
	xx::Shared<Node> GetParent();
	void AddChild(xx::Shared<Node> const& node);
	void RemoveChild(xx::Shared<Node> const& node);
	void RemoveSelf();
	void MoveChild(xx::Shared<Node> const& node, size_t const& index);
	xx::Shared<Node> GetNode(std::string_view const& path) const;
};
