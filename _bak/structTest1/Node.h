#pragma once

#include "Ref.h"

struct SceneTree;
struct Node : Ref<Node> {
	/*********************************************************************/
	// constructs

	Node(SceneTree* const& tree);
	virtual ~Node();
	Node(Node const&) = delete;
	Node& operator=(Node const&) = delete;

	/*********************************************************************/
	// fields

	bool entered = false;
	int indexOfProcess = -1;
	SceneTree* const tree;
	xx::Weak<Node> parent;
	std::vector<xx::Shared<Node>> children;
	std::string name;
	std::unordered_map<std::string_view, std::pair<xx::Weak<Node>, MFuncData*>> signalReceivers;

	// todo: map< groupName, group* > for destructor auto remove ?

	/*********************************************************************/
	// generic events

	virtual void EnterTree();
	virtual void ExitTree();
	virtual void Ready();
	virtual void Process(float delta);

	void CallEnterTree();
	void CallExitTree();
	void CallReady();

	/*********************************************************************/
	// utils

	SceneTree& GetTree() const;
	xx::Shared<Node> GetParent();
	xx::Shared<Node> GetNode(std::string_view const& path) const;
	template<typename T, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> GetNode(std::string_view const& path) const;

	void AddToGroup(std::string_view const& gn);
	void RemoveFromGroup(std::string_view const& gn);
	bool IsInGroup(std::string_view const& gn) const;


	template<typename T, typename Name, typename ...Args, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> CreateChild(Name&& name, Args&&...args);
	template<typename T, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> AddChild(xx::Shared<T> const& node);
	void RemoveChild(xx::Shared<Node> const& node);
	void Remove();
	void QueueRemove();

	void MoveChild(xx::Shared<Node> const& node, size_t const& index);
	void MoveToLast();

	void PrintTreePretty(std::string const& prefix = "", bool const& last = true) const;

	void EnableProcess(bool const& enable = true);

	// like AddToGroup( signalName ) but store  receiver + funcName
	void Connect(std::string_view const& signalName, xx::Shared<Node> const& receiver, std::string_view const& funcName);

	// like tree->CallGroup( signalName, receiver: funcName, ... )
	template<typename Name, typename ...Args>
	int EmitSignal(Name&& signalName, Args&&...args);
};
