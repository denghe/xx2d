#pragma once
#include "pch.h"

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
		Size maxSize;

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

		XY pos{ 0, 0 };
		XY anchor{ 0.5, 0.5 };
		XY scale{ 1, 1 };
		float radians{ 0 };
		bool flipX = false, flipY = false;
		RGBA8 color{ 255, 255, 255, 255 };

		/***************************************************************************/
		// change user data funcs

		// default anchor: 0, 1
		void SetText(BMFont bmf, std::string_view const& text, float const& fontSize = 32.f);

		void SetFlipX(bool const& fx);
		void SetFlipY(bool const& fy);

		void SetAnchor(XY const& a);

		void SetRotate(float const& r);

		void SetScale(XY const& s);
		void SetScale(float const& s);

		void SetPositon(XY const& p);
		void SetPositionX(float const& x);
		void SetPositionY(float const& y);

		void SetColor(RGBA8 const& c);

		void Commit();

		// todo: support trans
		void Draw();
		void Draw(AffineTransform const& t);
	};

}
