#pragma once
#include "xx_ptr.h"	// xx::Shared xx::Weak

struct SceneTree;
struct Node {
	/*********************************************************************/
	// constructs

	Node(SceneTree* tree);
	virtual ~Node() = default;
	Node(Node const&) = delete;
	Node& operator=(Node const&) = delete;

	/*********************************************************************/
	// fields

	SceneTree* const tree;
	bool entered = false;
	xx::Weak<Node> parent;
	std::vector<xx::Shared<Node>> children;
	std::string name;

	/*********************************************************************/
	// events

	virtual void EnterTree() {}
	virtual void ExitTree() {}
	virtual void Ready() {}
	virtual void Process(float delta) {}

	void CallEnterTree();
	void CallExitTree();
	void CallReady();
	void CallProcess(float delta);

	/*********************************************************************/
	// utils

	SceneTree* GetTree() const;
	xx::Shared<Node> GetParent();
	xx::Shared<Node> GetNode(std::string_view const& path) const;

	void AddChild(xx::Shared<Node> const& node);
	void RemoveChild(xx::Shared<Node> const& node);
	void Remove();

	void MoveChild(xx::Shared<Node> const& node, size_t const& index);
	void MoveToLast();

	void PrintTreePretty(std::string const& prefix = "", bool const& last = true) const;


	/*********************************************************************/
	// Shared / Weak utils

	XX_FORCEINLINE xx::PtrHeader* GetPtrHeader() const {
		return (xx::PtrHeader*)this - 1;
	}

	// unsafe
	template<typename T = Node>
	XX_FORCEINLINE xx::Weak<T> WeakFromThis() const {
		auto h = GetPtrHeader();
		assert((*(xx::Weak<T>*) & h).Lock().As<Node>());
		return *(xx::Weak<T>*)&h;
	}

	template<typename T = Node>
	XX_FORCEINLINE xx::Shared<T> SharedFromThis() const {
		return WeakFromThis<T>().Lock();
	}
};
