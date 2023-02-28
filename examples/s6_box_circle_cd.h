#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace BoxCircleCD {

	struct Scene;
	struct DragCircle {
		using LT = xx::MouseEventListener<DragCircle*>;
		bool HandleMouseDown(LT& L);
		int HandleMouseMove(LT& L);
		void HandleMouseUp(LT& L);

		void Init(Scene* const& owner, xx::XY const& pos, float const& radius, int32_t const& segments);

		Scene* owner{};
		xx::XY pos{}, dxy{};
		float radius{}, radiusPow2{};
		xx::LineStrip border;
	};

	struct DragBox {
		using LT = xx::MouseEventListener<DragBox*>;
		bool HandleMouseDown(LT& L);
		int HandleMouseMove(LT& L);
		void HandleMouseUp(LT& L);

		void Init(xx::XY const& pos, xx::XY const& size);

		Scene* owner{};
		xx::XY pos{}, size{};
		xx::LineStrip border;
	};

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		std::vector<DragCircle> cs;
		DragCircle::LT CL;
		DragCircle* draggingC{};

		size_t cursor{};
		std::vector<DragBox> bs;
		DragBox::LT BL;
		xx::Rnd rnd;
		double timePool{};
	};

}
