#pragma once
#include "SceneTree.h"
#include "Viewport.h"
#include "Node.h"
#include "Signal.h"

/* // godot example：

func _ready():
	add_to_group("enemies")

func _on_discovered(): # This is a purely illustrative function.
	get_tree().call_group("enemies", "player_was_discovered")
*/

struct Player : Node {
	using Node::Node;
	void Ready() override {
		GetTree().CallGroup("enemies", "PlayerWasDiscovered");
	}
};

struct Enemy : Node {
	Enemy(SceneTree* const& tree) : Node(tree) {
		AddToGroup("enemies");
	}
	void PlayerWasDiscovered() {
		std::cout << "PlayerWasDiscovered() called." << std::endl;
		assert(IsInGroup("enemies"));
		RemoveFromGroup("enemies");
		assert(!IsInGroup("enemies"));
	}
};

int main() {
	struct {
		int xxx = 12;
		bool eee = true;
	} s;
	if (auto&& [a, b] = s; b) {
		std::cout << a << std::endl;
	}


	SceneTree tree;
	{
		RegisterMethod(Enemy, PlayerWasDiscovered);
		auto e = tree.root->AddChild(tree.CreateNode<Enemy>("Enemy"));
		tree.root->AddChild(tree.CreateNode<Player>("Player"));
		tree.root->PrintTreePretty();
	}
	return tree.MainLoop();
}
