#pragma once
#include "pch.h"

namespace xx {

	struct Sprite {

		/***************************************************************************/
		// cache

		QuadVerts qv;
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
		float radians{ 0 };
		bool flipX = false, flipY = false;
		RGBA8 color{ 255, 255, 255, 255 };

		/***************************************************************************/
		// change user data funcs

		Sprite& SetTexture(xx::Shared<GLTexture> t);	// do not override anchor
		Sprite& SetFrame(xx::Shared<Frame> f, bool overrideAnchor = true);

		XY& Size() const;	// return frame.spriteSize

		Sprite& SetFlipX(bool const& fx);
		Sprite& SetFlipY(bool const& fy);

		Sprite& SetAnchor(XY const& a);

		Sprite& SetRotate(float const& r);
		Sprite& AddRotate(float const& r);	// radians += r

		Sprite& SetScale(XY const& s);
		Sprite& SetScale(float const& s);

		Sprite& SetPosition(XY const& p);
		Sprite& SetPositionX(float const& x);
		Sprite& SetPositionY(float const& y);
		Sprite& AddPosition(XY const& p);
		Sprite& AddPositionX(float const& x);
		Sprite& AddPositionY(float const& y);

		Sprite& SetColor(RGBA8 const& c);

		Sprite& SetParentAffineTransform(AffineTransform* const& t);

		void Commit();

		/***************************************************************************/
		// query funcs
		bool Empty() const;	// return !frame

		// todo coord convert 

		// need commit
		void Draw();
		void DrawWithoutCommit();
		void SubDraw();	// set dirtyParentAffineTransform = true & Draw()
		void Draw(AffineTransform const& t);
	};

}
