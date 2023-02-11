#pragma once
#include "pch.h"

namespace xx {
	struct Quad : QuadInstanceData {
		Shared<GLTexture> tex;
		Quad& SetTexture(xx::Shared<GLTexture> const& t);
		Quad& SetFrame(xx::Shared<Frame> const& f);
		void Draw() const;
	};
}
