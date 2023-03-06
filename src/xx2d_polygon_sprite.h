#pragma once
#include "xx2d_pch.h"

namespace xx {

	// support texture packer's polygon algorithm. more cpu usage
	struct PolygonSprite {

		/***************************************************************************/
		// cache

		std::vector<xx::XYUVRGBA8> vs;
		std::vector<uint16_t> is;
		AffineTransform at, atBak, * pat{};

		union {
			struct {
				uint8_t dirtyFrame;
				uint8_t dirtySizeAnchorPosScaleRotate;
				uint8_t dirtyColor;
				uint8_t dirtyParentAffineTransform;
			};
			uint32_t dirty = 0x00FFFFFFu;
		};

		/***************************************************************************/
		// user data

		xx::Shared<Frame> frame;
		XY pos{ 0, 0 };
		XY anchor{ 0.5, 0.5 };	// will be auto set by frame if included
		XY scale{ 1, 1 };
		XY flip{ 1, 1 };
		float radians{ 0 };
		RGBA8 color{ 255, 255, 255, 255 };

		/***************************************************************************/
		// change user data funcs

		PolygonSprite& SetTexture(xx::Shared<GLTexture> t);	// do not override anchor
		PolygonSprite& SetFrame(xx::Shared<Frame> f, bool overrideAnchor = true);

		XY& Size() const;	// return frame.spriteSize

		PolygonSprite& SetFlipX(bool const& fx);
		PolygonSprite& SetFlipY(bool const& fy);

		PolygonSprite& SetAnchor(XY const& a);

		PolygonSprite& SetRotate(float const& r);
		PolygonSprite& AddRotate(float const& r);	// radians += r

		PolygonSprite& SetScale(XY const& s);
		PolygonSprite& SetScale(float const& s);

		PolygonSprite& SetPosition(XY const& p);
		PolygonSprite& SetPositionX(float const& x);
		PolygonSprite& SetPositionY(float const& y);
		PolygonSprite& AddPosition(XY const& p);
		PolygonSprite& AddPositionX(float const& x);
		PolygonSprite& AddPositionY(float const& y);

		PolygonSprite& SetColor(RGBA8 const& c);

		PolygonSprite& SetParentAffineTransform(AffineTransform* const& t);

		void Commit();

		/***************************************************************************/
		// query funcs
		bool Empty() const;	// return !frame

		// todo coord convert 

		// need commit
		void Draw();
		void SubDraw();	// set dirtyParentAffineTransform = true & Draw()
		void Draw(AffineTransform const& t);
	};

}
