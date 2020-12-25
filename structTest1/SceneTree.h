#pragma once

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
	std::unordered_map<std::string_view, std::vector<xx::Weak<Node>>> groups;
	std::vector<xx::Weak<Node>> needRemoves;
	std::vector<Node*> processNodes;

	int MainLoop(float const& frameRate = 60);

	xx::Shared<Viewport>& GetRoot() { return root; }

	template<typename T, typename Name, typename ...Args, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> CreateNode(Name&& name, Args&&...args);

	template<typename ...Args>
	int CallGroup(std::string_view const& gn, std::string_view const& fn, Args&&...args);
};
