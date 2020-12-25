#include "All.h"

/*
// godot example：

func _ready():
	get_node("Button").connect("pressed", self, "_on_Button_pressed")

func _on_Button_pressed():
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
		// get_node("Button").connect("pressed", self, "_on_Button_pressed")
		GetNode("Button")->Connect("pressed", this, "OnButtonPressed");
	}
	void OnButtonPressed(/*xx::Shared<Node> const& sender*/) {
		// get_node("Label").text = "HELLO!"
		GetNode<Label>("Label")->text = "HELLO!";
	}
};

struct Button : Node {
	using Node::Node;
	typedef bool AutoEnableProcess;
	void Process(float delta) override {
		EmitSignal("pressed");	// fire button event
	}
};

int main() {
	SceneTree tree;
	{
		RegisterMethod("OnButtonPressed", &Canvas::OnButtonPressed);
		auto&& canvas = tree.CreateNode<Canvas>("Canvas");
		canvas->CreateChild<Button>("Button");
		canvas->CreateChild<Label>("Label");
		tree.root->AddChild(canvas);
		canvas->PrintTreePretty();
	}
	return tree.MainLoop();
}





//#include "SceneTree.h"
//#include "Viewport.h"
//#include "Node.h"
//#include "Signal.h"
//
//// todo: Connect + EmitSignal 似乎可以用类似 group 的工艺来做?
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
