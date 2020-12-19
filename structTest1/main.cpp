#pragma once
#include "SceneTree.h"
#include "Viewport.h"
#include "Node.h"

struct S : Node {
	S(SceneTree* tree, std::string const& name) : Node(tree) { this->name = name; std::cout << "new " << name << "()" << std::endl; }
	void EnterTree() override { std::cout << name << " EnterTree" << std::endl; }
	void ExitTree() override { std::cout << name << " ExitTree" << std::endl; }
	void Ready() override { std::cout << name << " Ready" << std::endl; }
	//void Process(float delta) override { std::cout << name << " Process delta = " << delta << std::endl; }
	~S() { std::cout << "~" << name << "()" << std::endl; }
};

float timer = -0.1f;
struct S1 : S {
	S1(SceneTree* tree) : S(tree, "S1") {}
	void Process(float delta) override {
		timer += delta;
		if (timer > 0) {
			PrintTreePretty();
			//MoveChild(GetNode("S2"), children.size() - 1);
			GetNode("S2")->MoveToLast();
			PrintTreePretty();

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
