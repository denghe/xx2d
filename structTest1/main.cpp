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

	xx::Shared<Node> GetParent();
	void AddChild(xx::Shared<Node> const& node);
	void RemoveChild(xx::Shared<Node> const& node);
	void Remove();
	// todo: GetNode("/root")
	// todo: print_tree_pretty   move_child
};

struct Viewport : Node {
	Viewport(SceneTree* tree) : Node(tree) {
		name = "root";
		entered = true;
		parent = WeakFromThis();
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
	auto iter = std::find(children.begin(), children.end(), node);
	if (iter == children.end()) throw std::runtime_error("RemoveChild error: can't find node in children");
	if (entered) {
		if (tree->removeProtect) throw std::runtime_error("RemoveChild error: removeProtected");
		tree->removeProtect = true;
		node->CallExitTree();
		tree->removeProtect = false;
	}
	children.erase(iter);
	node->parent.Reset();
}

void Node::Remove() {
	if (auto&& p = parent.Lock()) {
		p->RemoveChild(SharedFromThis());
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
	S(SceneTree* tree, std::string const& name) : Node(tree) { this->name = name; std::cout << "new " << name << "()" << std::endl;}
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
			Remove();
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
		std::cout << "------------ s1->AddChild(s2)" << std::endl;
		s1->AddChild(s2);
		std::cout << "------------ tree.root->AddChild(s1);" << std::endl;
		tree.root->AddChild(s1);
	}
	std::cout << "------------ MainLoop" << std::endl;
	return tree.MainLoop();
}
