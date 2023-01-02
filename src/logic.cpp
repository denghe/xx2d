#include "pch.h"
#include "logic.h"

void Logic::Init() {
	m.Fill(this, "res/tiledmap1/m1.tmx");
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;

	assert(m.layers[0]->type == TMX::LayerTypes::TileLayer);
	auto&& lt = *m.layers[0].ReinterpretCast<TMX::Layer_Tile>();
	assert(!m.infinite && lt.gids.size() == m.width * m.height);

	// calc display pixel width & height
	auto cw = this->w / m.width;
	auto ch = this->h / m.height;

	for (uint32_t cy = 0; cy < m.height; ++cy) {
		for (uint32_t cx = 0; cx < m.width; ++cx) {
			auto&& idx = cy * m.width + cx;
			assert(idx < lt.gids.size());
			auto&& gid = lt.gids[idx];
			assert(gid < m.gidInfos.size());
			auto&& i = m.gidInfos[gid];
			assert(i.texture);
			auto& qv = AutoBatchDrawQuadBegin(*i.texture);

			auto x = cx * cw - w / 2;
			auto y = h / 2 - cy * ch;

			qv[0].x = x;
			qv[0].y = y;
			qv[0].u = i.x;
			qv[0].v = i.y;

			qv[1].x = x;
			qv[1].y = y - ch;
			qv[1].u = i.x;
			qv[1].v = i.y + i.h;

			qv[2].x = x + cw;
			qv[2].y = y - ch;
			qv[2].u = i.x + i.w;
			qv[2].v = i.y + i.h;

			qv[3].x = x + cw;
			qv[3].y = y;
			qv[3].u = i.x + i.w;
			qv[3].v = i.y;

			qv[0].r = qv[0].g = qv[0].b = qv[0].a = 255;
			qv[1].r = qv[1].g = qv[1].b = qv[1].a = 255;
			qv[2].r = qv[2].g = qv[2].b = qv[2].a = 255;
			qv[3].r = qv[3].g = qv[3].b = qv[3].a = 255;

			AutoBatchDrawQuadEnd();
		}
	}

	return 0;
}
