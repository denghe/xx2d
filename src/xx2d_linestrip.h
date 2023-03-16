#pragma once
#include "xx2d.h"

namespace xx {

	struct LineStrip {
		std::vector<XYRGBA8> pointsBuf;
		AffineTransform at;
		bool dirty = true;

		std::vector<XY> points;
		XY size{};
		XY pos{};
		XY anchor{ 0.5, 0.5 };
		XY scale{ 1, 1 };
		float radians{};
		RGBA8 color{ 255, 255, 255, 255 };

		std::vector<XY>& SetPoints();
		LineStrip& SetPoints(std::initializer_list<XY> ps);

		LineStrip& FillCirclePoints(XY const& center, float const& radius, std::optional<float> const& angle = {}, int const& segments = 100, XY const& scale = { 1,1 });
		LineStrip& FillBoxPoints(XY const& center, XY const& wh);
		// ... more?

		LineStrip& Clear();

		LineStrip& SetSize(XY const& s);

		LineStrip& SetAnchor(XY const& a);

		LineStrip& SetRotate(float const& r);

		LineStrip& SetScale(XY const& s);
		LineStrip& SetScale(float const& s);

		LineStrip& SetPosition(XY const& p);

		LineStrip& SetColor(RGBA8 const& c);

		void Commit();

		void Draw();
		void Draw(AffineTransform const& t);
	};

}
