// 简单模拟 godot 的场景组织结构和事件
#include "SceneTree.h"
#include "Viewport.h"
#include "Node.h"

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
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr not found
			n = GetNode("./S2/S3/S4");
			std::cout << (n ? n->name : "nullptr") << std::endl;	// nullptr not found
			n = GetNode("/S1/S4");
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
