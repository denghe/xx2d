#pragma once
#include "pch.h"
#include "logic_base.h"

namespace xx {
	// current anchor always {0.5, 0.5}
	struct Quad : QuadInstanceData {
		Shared<GLTexture> tex;
		Quad& SetTexture(xx::Shared<GLTexture> const& t);
		Quad& SetFrame(xx::Shared<Frame> const& f);
		void Draw() const;
	};
}

struct Logic10 : LogicBase {
	void Init(Logic* logic) override;
	int Update() override;

	std::vector<xx::Quad> qs;
	xx::Rnd rnd;
};
