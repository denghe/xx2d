#pragma once
#include "main.h"

namespace MoreBoxCircleCD {

	struct Scene;
	struct C : xx::SpaceGridCItem<C> {
		Scene* owner{};
		xx::Pos<> newPos{};
		int32_t radius{}, speed{ 7 };
		xx::LineStrip border;

		void Init(Scene* const& owner_, xx::Pos<> const& pos, int32_t const& r, int32_t const& segments = 16);
		void SetPos(xx::Pos<> const& pos);
		void Update();
		void Update2();
		~C();
	};

	struct B : xx::SpaceGridABItem<B> {
		Scene* owner{};
		xx::Pos<> size{};
		xx::LineStrip border;

		void Init(Scene* const& owner_, xx::Pos<> const& pos, xx::Pos<> const& siz);
		~B();
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::SpaceGridC<C> sgc;
		xx::SpaceGridAB<B> sgab;
		xx::SpaceGridCCamera<C> cam;
		xx::Pos<> mousePos;	// store engine.mousePosition's grid world coordinate

		std::vector<xx::Shared<C>> cs;
		std::vector<xx::Shared<B>> bs;
		xx::Rnd rnd;
		double timePool{};
	};

}
