#pragma once
#include "Ref.h"

struct SceneTree;
struct Signal;
struct Node : Ref<Node> {
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

	// 似乎可以不需要这个？直接用个 Weak 变量存储某 signal 需要发给谁即可
	//std::unordered_map<std::string, xx::Weak<Node>> signalReceivers;

	//typedef void (Node::* SignalHandler)(Signal const& sig);
	//std::unordered_map<std::string, SignalHandler> signalHandlers;
	//void SignalHandle(Signal const& sig);

	virtual void Receive(Signal const& sig);

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
};
