#pragma once
#include "xx2d.h"

namespace xx {

	struct FpsViewer {
		SimpleLabel lbl;
		std::string extraInfo;
		int fps{}, counter{};
		double fpsTimePool{};

		// call at the update end
		void Draw(BMFont& fnt, float const& marginLeft = { 10 }, XY anchor = {}) {
			++counter;
			fpsTimePool += engine.delta;
			if (fpsTimePool >= 1) {
				fpsTimePool -= 1;
				fps = counter;
				counter = 0;
			}

			auto& sm = engine.sm;
			sm.End();		// commit

			auto xy = engine.ninePoints[1] + XY{ marginLeft, 10 };

			auto s = ToStringFormat("FPS = {0} DC = {1} VC = {2} PC = {3}; {4}"
				, fps, sm.drawCall, sm.drawVerts, sm.drawLinePoints, std::string_view(extraInfo));

			lbl.SetText(fnt, s, 32.f, engine.w - 10 - marginLeft)
				.SetAnchor(anchor)
				.SetPosition(xy.MakeAdd(2, -2))
				.SetColor({ 0, 0, 255, 255 })
				.Draw()		// shadow

				.SetPosition(xy)
				.SetColor({ 255, 0, 0, 255 })
				.Draw();	// body
		}
	};

}
