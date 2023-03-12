#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"
#include "xx2d_audio.h"

namespace AudioTest {

	struct Scene;
	struct Button;
	using ButtonMouseEventListener = xx::MouseEventListener<Button*>;

	struct Button {
		bool HandleMouseDown(ButtonMouseEventListener& L);
		int HandleMouseMove(ButtonMouseEventListener& L);
		void HandleMouseUp(ButtonMouseEventListener& L);

		void Init(Scene* const& scene_, xx::XY const& pos, int const& id_, std::string_view const& txt_, float const& fontSize);

		Scene* scene{};
		xx::XY leftBottom{}, rightTop{};	// bounding box
		std::string txt;
		int id{};


		bool Inside(xx::XY const& point);	// bounding box contains point?

		xx::LineStrip border;
		xx::Label content;
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::Audio audio;
		std::vector<Button> btns;
		ButtonMouseEventListener meListener;
	};

}
