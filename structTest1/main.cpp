#pragma once
#include "SceneTree.h"
#include "Viewport.h"
#include "Node.h"
#include "Signal.h"

#include "xx_chrono.h"


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
};

int main() {
	SceneTree tree;
	{
		auto&& canvas = tree.CreateNode<Canvas>("Canvas");
		canvas->AddChild(tree.CreateNode<Button>("Button"));
		canvas->AddChild(tree.CreateNode<Label>("Label"));

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
	// get_node("Label").text = "HELLO!"
	if (sender->name == "Button" && sig.name == "pressed") {
		GetNode("Label").As<Label>()->text = "HELLO! from Button";
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
