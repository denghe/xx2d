#pragma once
#include "pch.h"

namespace xx {

	struct SpriteNode : xx::Sprite {
		using Child = xx::Shared<SpriteNode>;
		using Children = std::vector<Child>;
		Children children;

		Child& operator[](size_t const& idx);

		void FillParentAffineTransaction();
		static void FillParentAffineTransaction(Children& cs, xx::AffineTransform* pat);

		void Draw();
		static void Draw(Children& cs);
	};

}
