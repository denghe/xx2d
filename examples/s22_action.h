﻿#pragma once
#include "main.h"

namespace ActionTest {

	struct Scene;

	struct Foo {
		Scene* scene{};
		std::optional<xx::Coro> action;
        xx::Quad body;
        xx::XY pos{};
        float radians{}, speed{};
		float alpha{ 1 }, scale{ 1 };
		bool dying{}, dead{};

        xx::Coro Action_Shake(float r1, float r2, float step);
        xx::Coro Action_MoveTo(xx::XY tar);
        xx::Coro Action_Shake_MoveTo(xx::XY tar);
        xx::Coro Action_FadeOut(float step);
        xx::Coro Action_ScaleTo(float s, float step);

        void Init(Scene* scene_);
		bool Update();
		void DrawInit();
		void Draw();
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

        xx::Shared<xx::GLTexture> tex;
		xx::ListLink<Foo> foos;
        float timePool{};
	};
}
