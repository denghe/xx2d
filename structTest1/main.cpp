#pragma once
#include "SceneTree.h"
#include "Viewport.h"
#include "Node.h"
#include "Signal.h"

#include "xx_chrono.h"



/*
// godot example：

func _ready():
	add_to_group("enemies")

func _on_discovered(): # This is a purely illustrative function.
	get_tree().call_group("enemies", "player_was_discovered")
*/

struct Player : Node {
	using Node::Node;
	void PlayerWasDiscovered() { 
		std::cout << "PlayerWasDiscovered() called" << std::endl; 
	}
	void PlayerWasDiscovered2(std::string xxx, float xxxx) { 
		std::cout << "PlayerWasDiscovered2(xxx, xxxx) called. xxx = " << xxx << ", xxxx = " << xxxx << std::endl;
	}
	void Ready() override {
		AddToGroup("enemies");
	}
};

struct Enemy : Node {
	using Node::Node;
	void Process(float delta) override {
		GetTree().CallGroup("enemies", "PlayerWasDiscovered");
		GetTree().CallGroup("enemies", "PlayerWasDiscovered2", "asdf", 1.2f);
	}
};

int main() {
	SceneTree tree;
	{
		RegisterMethod(Player, PlayerWasDiscovered);
		RegisterMethod(Player, PlayerWasDiscovered2);

		tree.root->AddChild(tree.CreateNode<Player>("Player"));
		tree.root->AddChild(tree.CreateNode<Enemy>("Enemy"));
		tree.root->PrintTreePretty();
	}
	return tree.MainLoop();
}
