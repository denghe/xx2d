#pragma once
#include "xx_ptr.h"	// xx::Shared xx::Weak
#include "Signal.h"

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


	int MainLoop();

	xx::Shared<Viewport>& GetRoot() { return root; }

	template<typename T, typename Name, typename ...Args, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> CreateNode(Name&& name, Args&&...args) {
		auto r = xx::MakeShared<T>(this, std::forward<Args>(args)...);
		r->name = std::forward<Name>(name);
		r.header()->ud = &TypeMFuncMappings<T>::map;	// 先在这里存储一下 函数映射指针
		return r;
	}

	template<typename ...Args>
	void CallGroup(std::string_view const& gn, std::string_view const& fn, Args&&...args) {
		auto iter = groups.find(gn);
		if (iter != groups.end()) {
			Signal s(fn, std::forward<Args>(args)...);
			auto& v = iter->second;
			for (int i = (int)v.size() - 1; i >= 0; --i) {
				if (auto&& c = v[i].Lock()) {
					auto&& map = *(MFuncMap*)(v[i].h->ud);
					auto&& [f, fp] = map[fn];
					f(c.pointer, fp, s);
				}
				else {
					// todo: 对于已失效的指针，与最后一个元素交换删除. 这将改变组内顺序
				}
			}
		}
	}
};
