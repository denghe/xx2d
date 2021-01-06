#include "All.h"

// todo: timer 部分的驱动应从帧逻辑移除， timer 似乎也不应该继承自 Node. 应该弄个 Timers 啥的，其子为精简结构的 timer

/*
// godot example：

func _ready():
	get_node("Timer").connect("timeout", self, "_on_Timer_timeout")

func _on_Timer_timeout():
	get_node("Label").text = "HELLO!"
*/

struct Label : Node {
	using Node::Node;
	std::string text;
	typedef bool AutoEnableProcess;	// auto call EnableProcess() after Create
	void Process(float delta) override {
		if (text == "HELLO!") {
			std::cout << text << std::endl;
			GetNode("/Canvas")->QueueRemove();
		}
	}
};

struct Canvas : Node {
	using Node::Node;
	void Ready() override {
		// get_node("Timer").connect("timeout", self, "_on_Timer_timeout")
		GetNode("Timer")->Connect("timeout", this, "OnTimerTimeout");
	}
	void OnTimerTimeout() {
		// get_node("Label").text = "HELLO!"
		GetNode<Label>("Label")->text = "HELLO!";
	}
};

int main() {
	SceneTree tree(60, 4096);	// 时间轮精度：每秒 60 次，长度 4096
	{
		REGISTER_METHODS(Canvas, OnTimerTimeout);
		//RegisterMethod("OnTimerTimeout", &Canvas::OnTimerTimeout);
		auto&& canvas = tree.CreateNode<Canvas>("Canvas");
		canvas->CreateChild<Timer>("Timer")->SetTimeout(2);	// timeout seconds = 2
		canvas->CreateChild<Label>("Label");
		tree.root->AddChild(canvas);
		canvas->PrintTreePretty();
	}
	return tree.MainLoop(100);	// 每帧模拟 100ms 延迟（实际 10帧每秒 )
}







//#include "All.h"
//
///*
//// godot example：
//
//func _ready():
//	get_node("Button").connect("pressed", self, "_on_Button_pressed")
//
//func _on_Button_pressed():
//	get_node("Label").text = "HELLO!"
//*/
//
//struct Label : Node {
//	using Node::Node;
//	std::string text;
//	typedef bool AutoEnableProcess;	// auto call EnableProcess() after Create
//	void Process(float delta) override {
//		if (text == "HELLO!") {
//			std::cout << text << std::endl;
//			GetNode("/Canvas")->QueueRemove();
//		}
//	}
//};
//
//struct Canvas : Node {
//	using Node::Node;
//	void Ready() override {
//		// get_node("Button").connect("pressed", self, "_on_Button_pressed")
//		GetNode("Button")->Connect("pressed", this, "OnButtonPressed");
//	}
//	void OnButtonPressed(/*xx::Shared<Node> const& sender*/) {
//		// get_node("Label").text = "HELLO!"
//		GetNode<Label>("Label")->text = "HELLO!";
//	}
//};
//
//struct Button : Node {
//	using Node::Node;
//	typedef bool AutoEnableProcess;
//	void Process(float delta) override {
//		EmitSignal("pressed");	// fire button event
//	}
//};
//
//int main() {
//	SceneTree tree;
//	{
//		RegisterMethod("OnButtonPressed", &Canvas::OnButtonPressed);
//		auto&& canvas = tree.CreateNode<Canvas>("Canvas");
//		canvas->CreateChild<Button>("Button");
//		canvas->CreateChild<Label>("Label");
//		tree.root->AddChild(canvas);
//		canvas->PrintTreePretty();
//	}
//	return tree.MainLoop();
//}







//#include "All.h"
//
///* // godot example：
//
//func _ready():
//	add_to_group("enemies")
//
//func _on_discovered(): # This is a purely illustrative function.
//	get_tree().call_group("enemies", "player_was_discovered")
//*/
//
//struct Player : Node {
//	using Node::Node;
//	using AutoEnableProcess = bool;
//	void Process(float delta) override {
//		if (!GetTree().CallGroup("enemies", "PlayerWasDiscovered")) {
//			QueueRemove();
//		}
//	}
//};
//
//struct Enemy : Node {
//	using Node::Node;
//	void Ready() override {
//		AddToGroup("enemies");
//	}
//	void PlayerWasDiscovered() {
//		std::cout << "Enemy's PlayerWasDiscovered() called." << std::endl;
//		QueueRemove();
//	}
//	void ExitTree() override {
//		std::cout << "Enemy ExitTree" << std::endl;
//	}
//	~Enemy() {
//		std::cout << "~Enemy" << std::endl;
//	}
//};
//
//int main() {
//	SceneTree tree;
//	{
//		RegisterMethod_(Enemy, PlayerWasDiscovered);
//		tree.root->AddChild(tree.CreateNode<Enemy>("Enemy"));
//		tree.root->AddChild(tree.CreateNode<Enemy>("Enemy"));
//		tree.root->AddChild(tree.CreateNode<Player>("Player"));
//		tree.root->PrintTreePretty();
//	}
//	return tree.MainLoop();
//}
