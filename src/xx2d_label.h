#pragma once
#include "xx2d.h"

namespace xx {

	struct Label {

		/***************************************************************************/
		// cache

		struct Char {
			xx::Shared<GLTexture> tex;
			QuadVerts qv;
			std::array<XY, 4> posBak;
		};
		std::vector<Char> chars;
		XY size;
		AffineTransform at;

		union {
			struct {
				uint8_t dirtyTextSizeAnchorPosScaleRotate;
				uint8_t dirtyColor;
				uint8_t dirtyDummy1;
				uint8_t dirtyDummy2;
			};
			uint32_t dirty = 0xFFFFFFFFu;
		};

		/***************************************************************************/
		// user data

		XY pos{};
		XY anchor{ 0.5, 0.5 };
		XY scale{ 1, 1 };
		float radians{};
		RGBA8 color{ 255, 255, 255, 255 };

		/***************************************************************************/
		// change user data funcs

		// default anchor: 0, 1
		Label& SetText(BMFont bmf, std::string_view const& text, float const& fontSize = 32.f, float const& lineWidthLimit = 0.f);

		Label& SetAnchor(XY const& a);

		Label& SetRotate(float const& r);

		Label& SetScale(XY const& s);
		Label& SetScale(float const& s);

		Label& SetPosition(XY const& p);
		Label& SetPositionX(float const& x);
		Label& SetPositionY(float const& y);

		Label& SetColor(RGBA8 const& c);

		void Commit();

		void Draw();
		void Draw(AffineTransform const& t);
	};

}
