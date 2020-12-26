#pragma once

inline SceneTree::SceneTree(float const& frameRate, size_t const& wheelLen)
	: frameRate(frameRate)
	, secondsPerFrame(1.f / frameRate)
{
	root.Emplace(this);
	beginSeconds = lastSeconds = xx::NowEpochSeconds();

	wheel.resize(wheelLen);
}

inline int SceneTree::MainLoop(int const& delayMS) {
	while (!root->children.empty()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(delayMS));	// simulate frame delay
		auto delta = (float)xx::NowEpochSeconds(lastSeconds);

		// step time wheel & call timers Timeout
		int cursorEnd = (int)((lastSeconds - beginSeconds) / secondsPerFrame) % ((int)wheel.size() - 1);
		int mod = (int)wheel.size() - 1;
		while (cursor != cursorEnd) {
			cursor = (cursor + 1) & mod;
			auto p = wheel[cursor];
			wheel[cursor] = nullptr;
			while (p) {
				auto np = p->timerNext;

				p->indexAtWheel = -1;
				p->timerPrev = nullptr;
				p->timerNext = nullptr;

				p->Timeout();
				p = np;
			};
		}

		// call nodes Process
		for (auto&& n : processNodes) {
			n->Process(delta);
		}

		// remove queued nodes
		for (auto&& o : removedNodes) {
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
				std::swap(v.back().h, v[i].h);
				v.pop_back();
			}
		}
		return (int)v.size();
	}
	return 0;
}
