#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace AudioTest {

	struct Audio {
		void* ctx{};
		Audio();
		~Audio();
		void Play(std::string_view fn);
	};

	struct Scene;
	struct Button;
	using ButtonMouseEventListener = xx::MouseEventListener<Button*>;

	struct Button {
		bool HandleMouseDown(ButtonMouseEventListener& L);
		int HandleMouseMove(ButtonMouseEventListener& L);
		void HandleMouseUp(ButtonMouseEventListener& L);

		void Init(Scene* const& scene, xx::XY const& pos, std::string_view const& txt, float const& fontSize);

		Scene* scene{};
		xx::XY leftBottom{}, rightTop{};	// bounding box
		std::string txt;

		bool Inside(xx::XY const& point);	// bounding box contains point?

		xx::LineStrip border;
		xx::Label content;
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		Audio audio;
		std::vector<Button> btns;
		ButtonMouseEventListener meListener;
	};

}
