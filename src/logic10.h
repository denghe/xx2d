#pragma once
#include "pch.h"
#include "logic_base.h"

namespace xx {
	struct Quad {
		QuadInstanceData qid{ {}, 1, 0, {255,255,255,255}, 0, 0, 0xFFFFu, 0xFFFFu };
		Shared<Frame> frame;
		Quad& SetTexture(xx::Shared<GLTexture> t);	// do not override anchor
		Quad& SetFrame(xx::Shared<Frame> f, bool overrideAnchor = true);
		XY& Size() const;	// return frame.spriteSize
		void Draw() const;
	};
}

struct Logic10 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	xx::Quad q;
};
