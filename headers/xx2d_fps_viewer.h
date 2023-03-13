#pragma once
#include "xx2d.h"

namespace xx {
	struct FpsViewer {
		xx::BMFont* fnt{};
		xx::SimpleLabel lbl;
		std::string extraInfo;
		int fps{}, counter{};
		double fpsTimePool{};

		void Init(xx::BMFont& fnt_) {
			fnt = &fnt_;
			lbl.SetAnchor({ 0, 0 });
		}

		// call at the end
		void Update() {
			++counter;
			fpsTimePool += xx::engine.delta;
			if (fpsTimePool >= 1) {
				fpsTimePool -= 1;
				fps = counter;
				counter = 0;
			}

			auto& sm = xx::engine.sm;
			sm.End();		// commit

			auto xy = xx::engine.ninePoints[1] + xx::XY{ 10, 10 };

			auto s = xx::ToStringFormat("fps = {0}, draw call = {1}, vert count = {2}, line point count = {3}; {4}"
				, fps, sm.drawCall, sm.drawVerts, sm.drawLinePoints, std::string_view(extraInfo));

			lbl.SetText(*fnt, s, 32.f, xx::engine.w - 40)
				.SetPosition(xy.MakeAdd(2, -2))
				.SetColor({ 0, 0, 255, 255 })
				.Draw()		// shadow

				.SetPosition(xy)
				.SetColor({ 255, 0, 0, 255 })
				.Draw();	// body
		}
	};
}
