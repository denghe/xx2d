#include "Node.h"
#include "SceneTree.h"
#include "Viewport.h"
#include "Signal.h"

Node::Node(SceneTree* const& tree) : tree(tree) {
	if (!tree) throw std::runtime_error("first args: tree is nullptr");
}

void Node::EnterTree() {}
void Node::ExitTree() {}
void Node::Ready() {}
void Node::Process(float delta) {}

void Node::CallEnterTree() {
	EnterTree();
	entered = true;
	for (auto&& c : children) {
		c->CallEnterTree();
	}
}

void Node::CallExitTree() {
	for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
		(*iter)->entered = false;
		(*iter)->CallExitTree();
	}
	entered = false;
	ExitTree();
}

void Node::CallReady() {
	for (auto&& c : children) {
		c->CallReady();
	}
	Ready();
}

void Node::CallProcess(float delta) {
	Process(delta);
	for (auto&& c : children) {
		c->CallProcess(delta);
	}
}

XX_FORCEINLINE SceneTree& Node::GetTree() const {
	return *tree;
}

XX_FORCEINLINE xx::Shared<Node> Node::GetParent() {
	return parent.Lock();
}

xx::Shared<Node> Node::GetNode(std::string_view const& path) const {
	Node* rtv = (Node*)this;
	if (!path.size()) return *(xx::Shared<Node>*) & rtv;
	auto p = path.data();
	auto e = p + path.size();
	if (p[0] == '/') {
		rtv = tree->root.pointer;
		++p;
	}
LabBegin:
	if (p == e) return *(xx::Shared<Node>*) & rtv;
	if (p[0] == '/') return {};										// syntax error?
	if (p[0] == '.') {
		if (p + 1 == e) return *(xx::Shared<Node>*) & rtv;							// .
		if (p[1] == '.') {
			rtv = rtv->parent.Lock().pointer;
			if (p + 2 == e) {										// ..
				return *(xx::Shared<Node>*) & rtv;
			}
			if (p[2] == '/') {										// ../
				if (!rtv) return {};
				p += 3;
				goto LabBegin;
			}
			else return {};											// syntax error?
		}
		else if (p[1] == '/') {										// ./
			p += 2;
			goto LabBegin;
		}
		else return {};												// syntax error?
	}
	else {
		size_t j = 0;
		auto b = p;
		for (; b < e; ++b) {
			if (*b == '/') {										// name/
				j = 1;
				break;
			}
			else if (*b == '.') return {};							// syntax error?
		}
		auto s = (size_t)(b - p);
		for (auto& c : rtv->children) {
			if (c->name.size() == s && memcmp(c->name.data(), p, s) == 0) {
				rtv = c.pointer;
				p = b + j;
				goto LabBegin;
			}
		}
		return {};													// not found
	}
}


void Node::AddToGroup(std::string_view const& gn) {
	tree->groups[gn].emplace_back(WeakFromThis());
}

void Node::RemoveFromGroup(std::string_view const& gn) {
	auto&& g = tree->groups[gn];
	auto&& w = WeakFromThis();
	if (auto&& iter = std::find(g.begin(), g.end(), w); iter != g.end()) {
		g.erase(iter);
	}
}

bool Node::IsInGroup(std::string_view const& gn) const {
	auto&& g = tree->groups[gn];
	auto&& w = WeakFromThis();
	return std::find(g.begin(), g.end(), w) != g.end();
}


void Node::RemoveChild(xx::Shared<Node> const& node) {
	if (GetPtrHeader() != node->parent.h) throw std::runtime_error("RemoveChild error: bad parent??");
	// find index
	auto siz = children.size();
	auto buf = (Node**)children.data();
	size_t i = 0;
	for (; i < siz; i++) {
		if (buf[i] == node.pointer) {
			break;
		}
	}
	if (i == siz) throw std::runtime_error("RemoveChild error: can't find node in children");
	if (entered) {
		if (tree->removeProtect) throw std::runtime_error("RemoveChild error: removeProtected");
		tree->removeProtect = true;
		node->CallExitTree();
		tree->removeProtect = false;
	}
	// fast move to last and pop
	memmove(buf + i, buf + i + 1, (siz - i - 1) * sizeof(Node*));
	buf[siz - 1] = node.pointer;
	children.pop_back();
	node->parent.Reset();
}

void Node::Remove() {
	if (auto&& p = parent.Lock()) {
		p->RemoveChild(SharedFromThis());
	}
}

void Node::QueueRemove() {
	// todo: 将 weak 指针放入 vector 待帧末 remove from parent
}

void Node::MoveChild(xx::Shared<Node> const& node, size_t const& index) {
	if (WeakFromThis() != node->parent) throw std::runtime_error("Find error: bad parent??");
	if (entered && tree->removeProtect) throw std::runtime_error("MoveChild error: removeProtected");
	auto siz = children.size();
	if (index >= siz) throw std::runtime_error("MoveChild error: index out of range");
	auto buf = (Node**)children.data();
	size_t i = 0;
	for (; i < siz; i++) {
		if (buf[i] == node.pointer) {
			break;
		}
	}
	if (i == siz) throw std::runtime_error("MoveChild error: can't find node in children");
	if (i == index) return;
	else if (i > index) {
		memmove(buf + index + 1, buf + index, (i - index) * sizeof(Node*));
	}
	else {
		memmove(buf + i, buf + i + 1, (index - i) * sizeof(Node*));
	}
	buf[index] = node.pointer;
}

void Node::MoveToLast() {
	auto self = this;
	if (auto&& p = parent.Lock()) {
		p->MoveChild(*(xx::Shared<Node>*)&self, p->children.size() - 1);
	}
}

void Node::PrintTreePretty(std::string const& prefix, bool const& last) const {
	std::cout << prefix + (last ? " L-" : " |-") + name << std::endl;
	for (auto& c : children) {
		c->PrintTreePretty(prefix + (last ? "   " : " | "), c == children.back());
	}
}

void Node::Receive(xx::Shared<Node> const& sender, Signal const& s) {
	std::cout << "Receive " << sender->name << "'s signal: " << s.name << std::endl;
}

void Node::Connect(std::string_view const& signalName, xx::Shared<Node> const& receiver) {
	auto&& iter = signalReceivers.find(signalName);
	if (iter != signalReceivers.end()) {
		iter->second = receiver;
	}
}
