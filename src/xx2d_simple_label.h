#pragma once
#include "xx2d.h"

namespace xx {

	// only support 1 texture, draw by quad instance, does not support AffineTransform, rotate
	struct SimpleLabel {

		/***************************************************************************/
		// cache

		xx::Shared<GLTexture> tex;
		struct Char {
			XY pos;
			uint16_t tx, ty, tw, th;
		};
		std::vector<Char> chars;
		XY size{};

		/***************************************************************************/
		// user data

		XY pos{};
		XY anchor{ 0.5, 0.5 };
		XY scale{ 1, 1 };
		float baseScale{ 1 };
		RGBA8 color{ 255, 255, 255, 255 };

		/***************************************************************************/
		// change user data funcs

		// default anchor: 0, 1
		SimpleLabel& SetText(BMFont bmf, std::string_view const& text, float const& fontSize = 32.f, float const& lineWidthLimit = 0.f);

		SimpleLabel& SetAnchor(XY const& a);

		SimpleLabel& SetScale(XY const& s);
		SimpleLabel& SetScale(float const& s);

		SimpleLabel& SetPosition(XY const& p);
		SimpleLabel& SetPositionX(float const& x);
		SimpleLabel& SetPositionY(float const& y);

		SimpleLabel& SetColor(RGBA8 const& c);

		SimpleLabel& Draw();
	};

}
