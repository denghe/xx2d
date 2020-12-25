#include "SceneTree.h"

template<typename T, class>
xx::Shared<T> Node::GetNode(std::string_view const& path) const {
	return GetNode(path).As<T>();
}

template<typename T, typename Name, typename ...Args, class>
xx::Shared<T> Node::CreateChild(Name&& name, Args&&...args) {
	return AddChild(tree->CreateNode<T>(name, std::forward<Args>(args)...));
}

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
	auto&& [weak, funcData] = signalReceivers[name];
	if (auto&& receiver = weak.Lock()) {
		auto&& [f, fp] = *funcData;
		Signal s(name, std::forward<Args>(args)...);
		f(receiver.pointer, fp, s);
		return 1;
	}
	return 0;
}
