#pragma once
#include "SceneTree.h"
#include "Viewport.h"
#include "Node.h"
#include "Signal.h"


/*

// godot 示例代码：

func _ready():
	get_node("Button").connect("pressed", self, "_on_Button_pressed")

func _on_Button_pressed():
	get_node("Label").text = "HELLO!"

实现需求分析：
	发起方 有个接收者映射字典："信号", 接收者( Weak<Node>? )
	接收者 有处理函数映射字典："信号", 处理函数( 函数指针? )
	有字典相关 注册 & 反注册工具函数

	当 Button 发起方 产生 点击 事件时，会发出 "pressed" 信号。
	由于在 映射字典 中找到了相应条目，则常识联系接收方( .Lock() ), 并将信号塞给对方( 调用对方的接收入口函数 )

	如果接收者 已释放( Lock 后为空 ): 从字典移除该条目。 如果接收者 未释放 但未在 tree 中，则保留但不发出 ??
	接收者 收到 "pressed" 后，在 映射字典 中查找 并定位到相应的 处理函数，投递参数并调用。
	如果没有找到相应的处理函数，就算了。

	思考：有没有可能一对多发送? 顺序? 需要返回值来告知是否已 handled 以阻止继续向下传递?
*/


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

			GetNode("S4")->RecvSignal({ "test", 1.2, std::string("asdf"), 345 });

			//Remove();
			timer = -100;
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

	void RecvSignal(Signal const& s) override {
		auto& a = s.args;
		if (s.name == "test" && a.size() == 3) {
			test(std::get<double>(a[0]), std::get<std::string>(a[1]), std::get<int32_t>(a[2]));
		}
	}

	void test(double const& a, std::string const& b, int const& c) {
		std::cout << "recv signal: test, args = " << a << " " << b << " " << c << std::endl;
	}
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
