#pragma once
#include "pch.h"

namespace xx {

	struct LineStrip {
		std::vector<XYRGBA8> pointsBuf;
		AffineTransform at;
		bool dirty = true;

		std::vector<XY> points;
		Size size;
		XY pos{ 0, 0 };
		XY anchor{ 0.5, 0.5 };
		XY scale{ 1, 1 };
		float radians{ 0 };
		RGBA8 color{ 255, 255, 255, 255 };

		std::vector<XY>& SetPoints();

		void FillCirclePoints(XY const& center, float const& radius, std::optional<float> const& angle = {}, int const& segments = 100, XY const& scale = { 1,1 });
		void FillBoxPoints(XY const& center, XY const& wh);

		void SetSize(Size const& s);

		void SetAnchor(XY const& a);

		void SetRotate(float const& r);

		void SetScale(XY const& s);
		void SetScale(float const& s);

		void SetPositon(XY const& p);

		void SetColor(RGBA8 const& c);

		void Commit();

		void Draw();
		void Draw(AffineTransform const& t);
	};

}
