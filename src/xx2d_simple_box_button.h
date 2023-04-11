#pragma once
#include "xx2d.h"

namespace xx {
	struct SimpleBoxButton;
	using MouseEventListener_SimpleBoxButton = xx::MouseEventListener<SimpleBoxButton*>;

	struct SimpleBoxButton {

		bool HandleMouseDown(MouseEventListener_SimpleBoxButton& L) {
			return Inside(L.downPos);
		}

		int HandleMouseMove(MouseEventListener_SimpleBoxButton& L) {
			return 0;
		}

		void HandleMouseUp(MouseEventListener_SimpleBoxButton& L) {
			if (Inside(xx::engine.mousePosition)) {
				handler();
			}
		}

		void Init(BMFont& fnt, xx::XY const& pos, std::string_view const& txt, float const& fontSize, std::function<void()> handler_) {
			handler = std::move(handler_);
			content.SetText(fnt, txt, fontSize);
			auto siz = GetSize();
			border.FillBoxPoints({}, siz);
			SetPosition(pos);
		}

		float paddingLeft{ 10 }, paddingRight{ 10 }, paddingTop{ 2 }, paddingBottom{ 2 };

		std::function<void()> handler;

		xx::XY leftBottom{}, rightTop{};	// bounding box
		bool Inside(xx::XY const& p) {
			return p.x >= leftBottom.x && p.x <= rightTop.x
				&& p.y >= leftBottom.y && p.y <= rightTop.y;
		}

		xx::LineStrip border;
		xx::SimpleLabel content;

		xx::XY GetSize() const {
			return content.size + xx::XY{ paddingLeft + paddingRight, paddingTop + paddingBottom };
		}

		void SetPosition(xx::XY const& pos) {
			auto siz = GetSize();
			siz /= 2;
			leftBottom = pos - siz;
			rightTop = pos + siz;
			border.SetPosition(pos);
			content.SetPosition(pos);
		}

		void Draw1() {
			border.Draw();
		}

		void Draw2() {
			content.Draw();
		}
	};
}
