#include "SceneTree.h"

template<typename T, class>
xx::Shared<T> Node::AddChild(xx::Shared<T> const& node) {
	if (node->parent) throw std::runtime_error("AddChild error: already have parent");
	if (node->entered) throw std::runtime_error("AddChild error: already entered");
	node->parent = WeakFromThis();
	children.push_back(node);
	if (entered) {
		tree->removeProtect = true;
		node->CallEnterTree();
		node->CallReady();
		tree->removeProtect = false;
	}
	return node;
}

template<typename Name, typename ...Args>
int Node::EmitSignal(Name&& name, Args&&...args) {
	auto&& [weak, fn] = signalReceivers[name];
	if (auto&& receiver = weak.Lock()) {
		Signal s(fn, std::forward<Args>(args)...);
		auto&& map = *(MFuncMap*)(weak.h->ud);
		auto&& [f, fp] = map[fn];
		f(receiver.pointer, fp, s);
		return 1;
	}
	return 0;
}
