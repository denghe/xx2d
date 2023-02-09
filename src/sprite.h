#pragma once
#include "pch.h"

namespace xx {

	struct Sprite {

		/***************************************************************************/
		// cache

		QuadVerts qv;
		AffineTransform at;

		union {
			struct {
				uint8_t dirtyFrame;
				uint8_t dirtySizeAnchorPosScaleRotate;
				uint8_t dirtyColor;
				uint8_t dirtyDummy;
			};
			uint32_t dirty = 0xFFFFFFFFu;
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

		void SetTexture(xx::Shared<GLTexture> t);	// do not override anchor

		void SetFrame(xx::Shared<Frame> f, bool overrideAnchor = true);

		void SetFlipX(bool const& fx);
		void SetFlipY(bool const& fy);

		void SetAnchor(XY const& a);

		void SetRotate(float const& r);

		void SetScale(XY const& s);
		void SetScale(float const& s);

		void SetPosition(XY const& p);
		void SetPositionX(float const& x);
		void SetPositionY(float const& y);

		void SetColor(RGBA8 const& c);

		void Commit();

		/***************************************************************************/
		// query funcs
		bool Empty() const;	// return !frame

		// todo coord convert 

		// need commit
		void Draw();
		void Draw(AffineTransform const& t);
	};

}
