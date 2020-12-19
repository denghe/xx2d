#include "SceneTree.h"

template<typename T, class>
xx::Shared<T> const& Node::AddChild(xx::Shared<T> const& node) {
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
void Node::EmitSignal(Name&& name, Args&&...args) {
	if (auto&& r = signalReceivers[name].Lock()) {
		r->Receive(SharedFromThis(), { std::forward<Name>(name), std::forward<Args>(args)... });
	}
}
