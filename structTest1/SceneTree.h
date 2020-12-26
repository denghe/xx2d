#pragma once

struct SceneTree;
struct Viewport;
struct Node;
struct Timer;

struct SceneTree {
	SceneTree(float const& frameRate = 60.f, size_t const& wheelLen = (1u << 12u));	// 60fps * 60sec = 3xxx < 4096
	SceneTree(SceneTree const&) = delete;
	SceneTree& operator=(SceneTree const&) = delete;

	bool removeProtect = false;
	int cursor = 0;
	std::vector<Timer*> wheel;
	double beginSeconds = 0;
	double lastSeconds = 0;
	double frameRate = 60.f;
	double secondsPerFrame = 1.0f / 60.f;
	xx::Shared<Viewport> root;
	std::unordered_map<std::string_view, std::vector<xx::Weak<Node>>> groups;
	std::vector<xx::Weak<Node>> removedNodes;
	std::vector<Node*> processNodes;



	int MainLoop(int const& delayMS = 16);

	xx::Shared<Viewport>& GetRoot() { return root; }

	template<typename T, typename Name, typename ...Args, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> CreateNode(Name&& name, Args&&...args);

	template<typename ...Args>
	int CallGroup(std::string_view const& gn, std::string_view const& fn, Args&&...args);
};
