#pragma once
#include "SceneTree.h"
#include "Viewport.h"
#include "Node.h"
#include "Signal.h"

// todo: Connect + EmitSignal 似乎可以用类似 group 的工艺来做?

/* // godot example：

func _ready():
	add_to_group("enemies")

func _on_discovered(): # This is a purely illustrative function.
	get_tree().call_group("enemies", "player_was_discovered")
*/

struct Player : Node {
	using Node::Node;
	void Process(float delta) override {
		if (!GetTree().CallGroup("enemies", "PlayerWasDiscovered")) {
			QueueRemove();
		}
	}
};

struct Enemy : Node {
	Enemy(SceneTree* const& tree) : Node(tree) {
		AddToGroup("enemies");
	}
	void PlayerWasDiscovered() {
		std::cout << "Enemy's PlayerWasDiscovered() called." << std::endl;
		QueueRemove();
	}
	void ExitTree() override {
		std::cout << "Enemy ExitTree" << std::endl;
	}
	~Enemy() {
		std::cout << "~Enemy" << std::endl;
	}
};

int main() {
	SceneTree tree;
	{
		RegisterMethod(Enemy, PlayerWasDiscovered);
		tree.root->AddChild(tree.CreateNode<Enemy>("Enemy"));
		tree.root->AddChild(tree.CreateNode<Enemy>("Enemy"));
		tree.root->AddChild(tree.CreateNode<Player>("Player"));
		tree.root->PrintTreePretty();
	}
	return tree.MainLoop();
}
