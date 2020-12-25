#pragma once

inline Viewport::Viewport(SceneTree* const& tree) : Node(tree) {
	name = "root";
	entered = true;
}
