#pragma once
#include "pch.h"

namespace xx {
	struct Quad : QuadInstanceData {
		Shared<GLTexture> tex;
		Quad& SetTexture(xx::Shared<GLTexture> const& t);
		Quad& SetFrame(xx::Shared<Frame> const& f);

		XY Size() const;	// return tex.wh

		Quad& SetAnchor(XY const& a);

		Quad& SetRotate(float const& r);
		Quad& AddRotate(float const& r);	// radians += r

		Quad& SetScale(XY const& s);
		Quad& SetScale(float const& s);

		Quad& SetPosition(XY const& p);
		Quad& SetPositionX(float const& x);
		Quad& SetPositionY(float const& y);
		Quad& AddPosition(XY const& p);
		Quad& AddPositionX(float const& x);
		Quad& AddPositionY(float const& y);

		Quad& SetColor(RGBA8 const& c);

		void Draw() const;
	};
}
