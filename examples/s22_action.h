#pragma once
#include "main.h"

namespace ActionTest {

	struct Scene;

	struct Foo {
		Scene* scene{};
		std::optional<xx::Coro> action;
        xx::Quad body;
        xx::XY pos{};
        float radians{}, speed{};

        xx::Coro Action_Shake();
        xx::Coro Action_MoveTo(xx::XY tar);
        xx::Coro Action_Shake_MoveTo(xx::XY tar);

        void Init(Scene* scene_);
        int Update();
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
