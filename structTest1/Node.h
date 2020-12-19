#pragma once
#include "Ref.h"

struct SceneTree;
struct Signal;
struct Node : Ref<Node> {
	/*********************************************************************/
	// constructs

	Node(SceneTree* const& tree);
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
	std::unordered_map<std::string_view, xx::Weak<Node>> signalReceivers;

	/*********************************************************************/
	// generic events

	virtual void EnterTree();
	virtual void ExitTree();
	virtual void Ready();
	virtual void Process(float delta);

	void CallEnterTree();
	void CallExitTree();
	void CallReady();
	void CallProcess(float delta);

	/*********************************************************************/
	// utils

	SceneTree* GetTree() const;
	xx::Shared<Node> GetParent();
	xx::Shared<Node> GetNode(std::string_view const& path) const;

	template<typename T, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> const& AddChild(xx::Shared<T> const& node);
	void RemoveChild(xx::Shared<Node> const& node);
	void Remove();

	void MoveChild(xx::Shared<Node> const& node, size_t const& index);
	void MoveToLast();

	void PrintTreePretty(std::string const& prefix = "", bool const& last = true) const;

	// signal funcs
	void Connect(std::string_view const& signalName, xx::Shared<Node> const& receiver);
	virtual void Receive(xx::Shared<Node> const& sender, Signal const& sig);
	template<typename Name, typename ...Args>
	void EmitSignal(Name&& name, Args&&...args);
	// register: signalReceivers["pressed"] = {};
	// register & cache: ptr = &signalReceivers.insert({ "pressed" , {} }).first->second;
	// fast emit: if (auto r = ptr->Lock()) r->Receive( SharedFromThis(), { "pressed", ...... });
};

#include "Node.hh"
