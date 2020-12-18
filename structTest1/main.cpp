// 简单模拟 godot 的场景组织结构和事件
#include <thread>
#include <vector>
#include <chrono>
#include "xx_ptr.h"	// xx::Shared xx::Weak

struct SceneTree;
struct Viewport;
struct Node;

struct SceneTree {
	xx::Shared<Viewport> root;
	xx::Shared<Viewport>& GetRoot() { return root; }

	bool removeProtect = false;

	SceneTree();
	SceneTree(SceneTree const&) = delete;
	SceneTree& operator=(SceneTree const&) = delete;

	int MainLoop();

	template<typename T, typename ...Args, class = std::enable_if_t<std::is_base_of_v<Node, T>>>
	xx::Shared<T> CreateNode(Args&&...args);
};

struct Node {
	virtual ~Node() = default;
	Node(Node const&) = delete;
	Node& operator=(Node const&) = delete;

	template<typename T = Node>
	XX_FORCEINLINE xx::Weak<T> WeakFromThis() const {
		auto h = (xx::PtrHeader*)this - 1;
		return *((xx::Weak<T>*) & h);
	}
	template<typename T = Node>
	XX_FORCEINLINE xx::Shared<T> SharedFromThis() const {
		return WeakFromThis<T>().Lock();
	}

	SceneTree* const tree;
	Node(SceneTree* tree) : tree(tree) {
		if (!tree) throw std::runtime_error("first args: tree is nullptr");
	}
	SceneTree* GetTree() const;

	bool entered = false;
	xx::Weak<Node> parent;
	std::vector<xx::Shared<Node>> children;
	std::string name;

	virtual void EnterTree() {}
	virtual void ExitTree() {}
	virtual void Ready() {}
	virtual void Process(float delta) {}

	void CallEnterTree();
	void CallExitTree();
	void CallReady();
	void CallProcess(float delta);

	void PrintTreePretty(std::string const& prefix = "", bool const& last = true) const;
	xx::Shared<Node> GetParent();
	void AddChild(xx::Shared<Node> const& node);
	void RemoveChild(xx::Shared<Node> const& node);
	void RemoveSelf();
	void MoveChild(xx::Shared<Node> const& node, size_t const& index);
	xx::Shared<Node> GetNode(std::string_view const& path) const;
};

struct Viewport : Node {
	Viewport(SceneTree* tree) : Node(tree) {
		name = "root";
		entered = true;
	}
};

SceneTree* Node::GetTree() const {
	return tree;
}

xx::Shared<Node> Node::GetParent() {
	return parent.Lock();
}

void Node::AddChild(xx::Shared<Node> const& node) {
	if (node->parent) throw std::runtime_error("AddChild error: already have parent");
	if (node->entered) throw std::runtime_error("AddChild error: already entered");
	node->parent = WeakFromThis<Node>();
	children.push_back(node);
	if (entered) {
		tree->removeProtect = true;
		node->CallEnterTree();
		node->CallReady();
		tree->removeProtect = false;
	}
}

void Node::CallEnterTree() {
	EnterTree();
	entered = true;
	for (auto&& c : children) {
		c->CallEnterTree();
	}
}

void Node::RemoveChild(xx::Shared<Node> const& node) {
	if (WeakFromThis() != node->parent) throw std::runtime_error("RemoveChild error: bad parent??");
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

void Node::RemoveSelf() {
	if (auto&& p = parent.Lock()) {
		p->RemoveChild(SharedFromThis());
	}
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

xx::Shared<Node> Node::GetNode(std::string_view const& path) const {
	Node* rtv = (Node*)this;
	if (!path.size()) return *(xx::Shared<Node>*)&rtv;
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
		if (p + 1 == e) return *(xx::Shared<Node>*) & rtv;			// .
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

void Node::PrintTreePretty(std::string const& prefix, bool const& last) const {
	std::cout << prefix + (last ? " L-" : " |-") + name << std::endl;
	for (auto& c : children) {
		c->PrintTreePretty(prefix + (last ? "   " : " | "), c == children.back());
	}
}

SceneTree::SceneTree() {
	root.Emplace(this);
}

int SceneTree::MainLoop() {
	while (true) {
		// simulate frame delay
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		float delta = 0.016f;
		root->CallProcess(delta);
	}
	return 0;
}

template<typename T, typename ...Args, class>
xx::Shared<T> SceneTree::CreateNode(Args&&...args) {
	return xx::MakeShared<T>(this, std::forward<Args>(args)...);
}



















float timer = -1.0f;
struct S : Node {
	S(SceneTree* tree, std::string const& name) : Node(tree) { this->name = name; std::cout << "new " << name << "()" << std::endl; }
	void EnterTree() override { std::cout << name << " EnterTree" << std::endl; }
	void ExitTree() override { std::cout << name << " ExitTree" << std::endl; }
	void Ready() override { std::cout << name << " Ready" << std::endl; }
	//void Process(float delta) override { std::cout << name << " Process delta = " << delta << std::endl; }
	~S() { std::cout << "~" << name << "()" << std::endl; }
};

struct S1 : S {
	S1(SceneTree* tree) : S(tree, "S1") {}
	void Process(float delta) override {
		timer += delta;
		if (timer > 0) {
			PrintTreePretty();
			std::cout << "test GetNode(path)" << std::endl;
			auto n = GetNode("/");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// root
			n = GetNode("/.");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// root
			n = GetNode("..");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// root
			n = GetNode("../");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// root
			n = GetNode("/..");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr
			n = GetNode("//");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr
			n = GetNode(".");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S1
			n = GetNode("..");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// root
			n = GetNode("S2");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S2
			n = GetNode("./S2");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S2
			n = GetNode("./S2/S3");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S2
			n = GetNode("./S2/S2");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr
			n = GetNode("./S2/S3/S4");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr
			n = GetNode("../S1/S4");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S4
			n = GetNode("../S1/S4/../S2/S3/../../S4");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S4

			RemoveSelf();
		}
	}
};

struct S2 : S {
	S2(SceneTree* tree) : S(tree, "S2") {}
	void EnterTree() override;
};

struct S3 : S {
	S3(SceneTree* tree) : S(tree, "S3") {}
};

struct S4 : S {
	S4(SceneTree* tree) : S(tree, "S4") {}
};

void S2::EnterTree() {
	this->S::EnterTree();
	auto s3 = tree->CreateNode<S3>();
	std::cout << "------------ s2->AddChild(s3);" << std::endl;
	AddChild(s3);
}

int main() {


	SceneTree tree;
	{
		auto s1 = tree.CreateNode<S1>();
		auto s2 = tree.CreateNode<S2>();
		auto s4 = tree.CreateNode<S4>();

		std::cout << "------------ s1->AddChild(s2)" << std::endl;
		s1->AddChild(s2);

		std::cout << "------------ tree.root->AddChild(s1);" << std::endl;
		tree.root->AddChild(s1);

		std::cout << "------------ s1->AddChild(s4);" << std::endl;
		s1->AddChild(s4);
	}
	std::cout << "------------ MainLoop" << std::endl;
	return tree.MainLoop();
}
