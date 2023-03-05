#pragma once
#include "xx2d_pch.h"
#include "scene_base.h"

namespace PolygonSprite {

	struct Scene : SceneBase {
		xx::Shared<xx::GLTexture> tex;
		std::vector<xx::XYUVRGBA8> vs;
		std::vector<uint16_t> is;
		void Init(GameLooper* looper) override;
		int Update() override;
	};

}
