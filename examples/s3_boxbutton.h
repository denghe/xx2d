#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace BoxButton {

	struct Button;
	using ButtonMouseEventListener = xx::MouseEventListener<Button*>;

	struct Button {
		bool HandleMouseDown(ButtonMouseEventListener& L);
		int HandleMouseMove(ButtonMouseEventListener& L);
		void HandleMouseUp(ButtonMouseEventListener& L);

		void Init(GameLooper* const& looper, xx::XY const& pos, xx::XY const& borderSize, std::string_view const& txt, float const& fontSize);

		GameLooper* looper;
		xx::XY leftBottom{}, rightTop{};	// bounding box
		std::string txt;

		bool Inside(xx::XY const& point);	// bounding box contains point?

		xx::LineStrip border;
		xx::Label content;
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		std::vector<Button> btns;
		ButtonMouseEventListener meListener;
	};

}
