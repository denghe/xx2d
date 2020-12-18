// 简单模拟 godot 的场景组织结构和事件
#include <thread>
#include <vector>
#include <chrono>
#include "xx_ptr.h"	// xx::Shared xx::Weak
#include "xx_chrono.h"
#include "string_text_view.hpp"

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

struct NodePathEx;
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
	xx::Shared<Node> GetNodeEx(std::initializer_list<std::string_view> paths) const;
	xx::Shared<Node> GetNodeEx2(std::vector<std::string> const& paths) const;
	xx::Shared<Node> GetNodeEx3(NodePathEx const& paths) const;
	xx::Shared<Node> GetNodeEx4(std::string_view const& paths) const;
};

struct Viewport : Node {
	Viewport(SceneTree* tree) : Node(tree) {
		name = "root";
		entered = true;
	}
};

struct NodePath {
	Node const* c;
	NodePath(Node const* const& c) : c(c) {}
	NodePath& operator/(std::string_view const& path) {
		if (c) {
			if (path == "..") {
				c = c->parent.Lock().pointer;
			}
			else {
				for (auto& o : c->children) {
					if (o->name == path) {
						c = o.pointer;
						return *this;
					}
				}
				c = nullptr;	// not found
			}
		}
		return *this;
	}
	operator xx::Shared<Node> const& () {
		return *(xx::Shared<Node>*) & c;
	}
};

struct NodePathEx {
	std::string path;
	std::vector<std::string_view> split;	// split by '/'
	NodePathEx(std::string_view const& paths) : path(paths) {
		if (!path.size()) return;
		auto p = path.data();
		auto e = p + path.size();
		if (p[0] == '/') {
			split.emplace_back(p, 1);
			++p;
		}
		auto b = p;
		for (; b != e; ++b) {
			if (*b == '/') {
				if (b != p) {
					split.emplace_back(p, b - p);
					p = b + 1;
					++b;
				}
			}
		}
		if (b != p) {
			split.emplace_back(p, b - p);
		}
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

xx::Shared<Node> Node::GetNodeEx(std::initializer_list<std::string_view> paths = {}) const {
	auto c = (Node*)this;
	if (!paths.size()) return *(xx::Shared<Node>*) & c;
	auto iter = paths.begin();
	if (*iter == "/") {
		c = tree->root.pointer;
		++iter;
	}
	NodePath np(c);
	for (; iter != paths.end(); ++iter) {
		np / *iter;
	}
	return np;
}

xx::Shared<Node> Node::GetNodeEx2(std::vector<std::string> const& paths) const {
	auto c = (Node*)this;
	auto siz = paths.size();
	auto buf = paths.data();
	if (!paths.size()) return *(xx::Shared<Node>*) & c;
	size_t i = 0;
	if (paths[i] == "/") {
		c = tree->root.pointer;
		++i;
	}
	NodePath np(c);
	for (; i != siz; ++i) {
		np / buf[i];
	}
	return np;
}

xx::Shared<Node> Node::GetNodeEx3(NodePathEx const& paths) const {
	auto& s = paths.split;
	if (s.empty()) return {};
	size_t i = 0, siz = s.size();
	auto c = (Node*)this;
	if (s[0].size() == 1 && s[0][0] == '/') {
		c = tree->root.pointer;
		i = 1;
	}
	for (; i < siz; ++i) {
		auto&& path = s[i];
		auto pathSiz = path.size();
		if (pathSiz == 2 && path[0] == '.' && path[1] == '.') {
			c = c->parent.Lock().pointer;
			if (!c) break;
		}
		else {
			auto bak = c;
			for (auto& o : c->children) {
				if (o->name.size() == pathSiz && memcmp(o->name.data(), path.data(), pathSiz) == 0) {
					c = o.pointer;
					break;
				}
			}
			if (bak == c) {
				c = nullptr;
				break;
			}
		}
	}
	return *(xx::Shared<Node>*) & c;
}

xx::Shared<Node> Node::GetNodeEx4(std::string_view const& paths) const {
	string_text_view stv{ paths,'/' };
	auto c = (Node*)this;
	if (paths[0] == '/') {
		c = tree->root.pointer;
	}
	for (auto&& path : stv) {
		if (!path.size()) continue;
		if (path.size() == 2 && path[0] == '.' && path[1] == '.') {
			c = c->parent.Lock().pointer;
			if (!c) break;
		}
		else {
			auto bak = c;
			for (auto& o : c->children) {
				if (o->name.size() == path.size() && memcmp(o->name.data(), path.data(), path.size()) == 0) {
					c = o.pointer;
					break;
				}
			}
			if (bak == c) {
				c = nullptr;
				break;
			}
		}
	}
	return *(xx::Shared<Node>*) & c;
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

struct S1__________ : S {
	S1__________(SceneTree* tree) : S(tree, "S1__________") {}
	void Process(float delta) override {
		timer += delta;
		if (timer > 0) {
			PrintTreePretty();
			// todo: test MoveNode

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
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr no parent
			n = GetNode("//");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr syntex error
			n = GetNode(".");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S1__________
			n = GetNode("..");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// root
			n = GetNode("S2__________");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S2__________
			n = GetNode("./S2__________");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S2__________
			n = GetNode("./S2__________/S3__________");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S2__________
			n = GetNode("./S2__________/S2__________");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr not found
			n = GetNode("./S2__________/S3__________/S4__________");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr not found
			n = GetNode("/S1__________/S4__________");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S4__________
			n = GetNode("../S1__________/S4__________/../S2__________/S3__________/../../S4__________");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S4__________

			std::cout << "test GetNodeEx3(path)" << std::endl;
			n = GetNodeEx3({ "../S1__________/S4__________/../S2__________/S3__________/../../S4__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S4__________
			n = GetNodeEx3({ "/S1__________/S4__________/../S2__________/S3__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S3__________

			std::cout << "test GetNodeEx4(path)" << std::endl;
			n = GetNodeEx4({ "../S1__________/S4__________/../S2__________/S3__________/../../S4__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S4__________
			n = GetNodeEx4({ "/S1__________/S4__________/../S2__________/S3__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S3__________

			std::cout << "test NodePath(node) / path / path ... " << std::endl;
			n = NodePath(this);
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S1__________
			n = NodePath(this) / "..";
			std::cout << (n ? n->name : "nullptr") << std::endl;	// root
			n = NodePath(this) / "S2__________";
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S2__________
			n = NodePath(this) / "S2__________" / "S3__________";
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S3__________
			n = NodePath(this) / "S2__________" / "S3__________" / "S4__________";
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr not found
			n = NodePath(tree->root) / "S1__________" / "S4__________";
			std::cout << (n ? n->name : "nullptr") << std::endl;	// s4

			std::cout << "test GetNodeEx({ name, name, .... })" << std::endl;
			n = GetNodeEx();
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S1__________
			n = GetNodeEx({ ".." });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// root
			n = GetNodeEx({ "S2__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S2__________
			n = GetNodeEx({ "S2__________", "S3__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S3__________
			n = GetNodeEx({ "S2__________", "S3__________", "S4__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr not found
			n = GetNodeEx({ "/", "S1__________", "S4__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S4__________

			std::cout << "test GetNodeEx2( names vector )" << std::endl;
			n = GetNodeEx2({});
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S1__________
			n = GetNodeEx2({ ".." });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// root
			n = GetNodeEx2({ "S2__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S2__________
			n = GetNodeEx2({ "S2__________", "S3__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S3__________
			n = GetNodeEx2({ "S2__________", "S3__________", "S4__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr not found
			n = GetNodeEx2({ "/", "S1__________", "S4__________" });
			std::cout << (n ? n->name : "nullptr") << std::endl;	// S4__________

			std::cout << "performance compare" << std::endl;

			auto seconds = xx::NowEpochSeconds();
			size_t i = 0;
			for (; i < 10000000; i++) {
				n = GetNode("/S1__________/S4__________/../S2__________/S3__________");
			}
			std::cout << "GetNode " << i << " times. elapsed seconds = " << (xx::NowEpochSeconds() - seconds) << std::endl;
			std::cout << (n ? n->name : "nullptr") << std::endl;	// s3

			i = 0;
			for (; i < 10000000; i++) {
				n = GetNodeEx({ "/", "S1__________", "S4__________", "..", "S2__________", "S3__________" });
			}
			std::cout << "GetNodeEx " << i << " times. elapsed seconds = " << (xx::NowEpochSeconds() - seconds) << std::endl;
			std::cout << (n ? n->name : "nullptr") << std::endl;	// s3

			i = 0;
			std::vector<std::string> paths{ "/", "S1__________", "S4__________", "..", "S2__________", "S3__________" };
			for (; i < 10000000; i++) {
				n = GetNodeEx2(paths);
			}
			std::cout << "GetNodeEx2 " << i << " times. elapsed seconds = " << (xx::NowEpochSeconds() - seconds) << std::endl;
			std::cout << (n ? n->name : "nullptr") << std::endl;	// s3

			i = 0;
			NodePathEx pathsex("/S1__________/S4__________/../S2__________/S3__________");
			for (; i < 10000000; i++) {
				n = GetNodeEx3(pathsex);
			}
			std::cout << "GetNodeEx3 " << i << " times. elapsed seconds = " << (xx::NowEpochSeconds() - seconds) << std::endl;
			std::cout << (n ? n->name : "nullptr") << std::endl;	// s3

			i = 0;
			for (; i < 10000000; i++) {
				n = GetNodeEx4("/S1__________/S4__________/../S2__________/S3__________");
			}
			std::cout << "GetNodeEx4 " << i << " times. elapsed seconds = " << (xx::NowEpochSeconds() - seconds) << std::endl;
			std::cout << (n ? n->name : "nullptr") << std::endl;	// s3

			i = 0;
			for (; i < 10000000; i++) {
				n = NodePath(tree->root) / "S1__________" / "S4__________" / ".." / "S2__________" / "S3__________";
			}
			std::cout << "NodePath " << i << " times. elapsed seconds = " << (xx::NowEpochSeconds() - seconds) << std::endl;
			std::cout << (n ? n->name : "nullptr") << std::endl;	// s3

			RemoveSelf();
		}
	}
};

struct S2__________ : S {
	S2__________(SceneTree* tree) : S(tree, "S2__________") {}
	void EnterTree() override;
};

struct S3__________ : S {
	S3__________(SceneTree* tree) : S(tree, "S3__________") {}
};

struct S4__________ : S {
	S4__________(SceneTree* tree) : S(tree, "S4__________") {}
};

void S2__________::EnterTree() {
	this->S::EnterTree();
	auto s3 = tree->CreateNode<S3__________>();
	std::cout << "------------ s2->AddChild(s3);" << std::endl;
	AddChild(s3);
}

int main() {


	SceneTree tree;
	{
		auto s1 = tree.CreateNode<S1__________>();
		auto s2 = tree.CreateNode<S2__________>();
		auto s4 = tree.CreateNode<S4__________>();

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
