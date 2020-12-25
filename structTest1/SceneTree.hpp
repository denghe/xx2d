#pragma once

inline SceneTree::SceneTree() {
	root.Emplace(this);
	beginSeconds = lastSeconds = xx::NowEpochSeconds();
}

inline int SceneTree::MainLoop() {
	while (!root->children.empty()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(16));	// simulate frame delay
		auto delta = (float)xx::NowEpochSeconds(lastSeconds);

		if (int i = (int)processNodes.size()) {
			for (--i; i >= 0; --i) {
				processNodes[i]->Process(delta);
			}
		}

		for (auto&& o : needRemoves) {
			if (o) {
				o->Remove();
			}
		}
	}
	return 0;
}

template<typename T, typename Name, typename ...Args, class>
xx::Shared<T> SceneTree::CreateNode(Name&& name, Args&&...args) {
	auto r = xx::MakeShared<T>(this, std::forward<Args>(args)...);
	r->name = std::forward<Name>(name);
	r.header()->ud = &TypeInfoMappings<T>::typeInfo;
	if constexpr (AutoEnableProcess<T>) {
		r->EnableProcess();
	}
	return r;
}

template<typename ...Args>
int SceneTree::CallGroup(std::string_view const& gn, std::string_view const& fn, Args&&...args) {
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
				// 与最后一个元素交换删除并 pop
				std::swap(v.back().h, v[i].h);
				v.pop_back();
			}
		}
		return (int)v.size();
	}
	return 0;
}
