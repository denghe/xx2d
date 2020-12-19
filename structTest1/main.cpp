#pragma once
#include "SceneTree.h"
#include "Viewport.h"
#include "Node.h"
#include "Signal.h"


/*
// godot example：

func _ready():
	get_node("Button").connect("pressed", self, "_on_Button_pressed")

func _on_Button_pressed():
	get_node("Label").text = "HELLO!"
*/

struct Canvas : Node {
	using Node::Node;
	void Ready() override;
	void Receive(xx::Shared<Node> const& sender, Signal const& sig) override;
};

struct Button : Node {
	Button(SceneTree* const& tree);
	void Process(float delta) override;
};

struct Label : Node {
	using Node::Node;
	std::string text;
	void Process(float delta) override;
};

int main() {
	SceneTree tree;
	{
		auto&& canvas = tree.CreateNode<Canvas>("Canvas");
		auto&& button = canvas->AddChild(tree.CreateNode<Button>("Button"));
		auto&& label = canvas->AddChild(tree.CreateNode<Label>("Label"));

		tree.root->AddChild(canvas);
		canvas->PrintTreePretty();
	}
	return tree.MainLoop();
}

void Canvas::Ready() {
	// get_node("Button").connect("pressed", self, "_on_Button_pressed")
	GetNode("Button").As<Button>()->Connect("pressed", this);
}

void Canvas::Receive(xx::Shared<Node> const& sender, Signal const& sig) {
	if (sender->name == "Button" && sig.name == "pressed") {
		// get_node("Label").text = "HELLO!"
		GetNode("Label").As<Label>()->text = "HELLO!";
	}
}

Button::Button(SceneTree* const& tree) : Node(tree) {
	// register & cache signal keyword
	signalReceivers["pressed"] = {};
}

void Button::Process(float delta) {
	// emit pressed signal
	EmitSignal("pressed");
}

void Label::Process(float delta) {
	if (!text.empty()) {
		// dump result & exit
		std::cout << text << std::endl;
		GetNode("../")->Remove();
	}
}
