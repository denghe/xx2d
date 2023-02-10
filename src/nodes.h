#pragma once
#include "pch.h"

namespace xx {

	struct SpriteNode : xx::Sprite {
		using Children = std::vector<xx::Shared<SpriteNode>>;
		Children children;

		void FillParentAffineTransaction();
		static void FillParentAffineTransaction(Children& cs, xx::AffineTransform* pat);

		void Draw();
		static void Draw(Children& cs);
	};

}
