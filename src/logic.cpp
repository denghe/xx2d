#include "pch.h"
#include "logic.h"

void TmxCamera::SetRange(Engine* eg, TMX::Map& map, Rect const& rect) {
	this->gridRect = rect;
	spriteSize.w = eg->w / rect.w;
	spriteSize.h = eg->h / rect.h;
	spriteScale.x = spriteSize.w / map.tileWidth;
	spriteScale.y = spriteSize.h / map.tileHeight;
}

void TmxCamera::SetPos(XY const& xy) {
	gridRect.x = xy.x;
	gridRect.y = xy.y;
}

void Logic::Init() {
	// for display drawcall
	fnt1 = LoadBMFont("res/font1/basechars.fnt"sv);
	lbCount.SetPositon(ninePoints[1] + XY{ 10, 10 });
	lbCount.SetAnchor({0, 0});

	// load map
	TMX::FillTo(map, this, "res/tiledmap1/m1.tmx");

	// todo: recursive scan & findout all tile layer & draw
	assert(map.layers[0]->type == TMX::LayerTypes::TileLayer);
	auto&& lt = *map.layers[0].ReinterpretCast<TMX::Layer_Tile>();
	assert(!map.infinite && lt.gids.size() == map.width * map.height);

	// convert all gids to sprites
	ss.resize(lt.gids.size());
	for (uint32_t cy = 0; cy < map.height; ++cy) {
		for (uint32_t cx = 0; cx < map.width; ++cx) {
			auto&& idx = cy * map.width + cx;
			auto&& gid = lt.gids[idx];
			assert(gid < map.gidInfos.size());
			auto&& i = map.gidInfos[gid];
			assert(i.texture);
			auto f = xx::Make<Frame>();
			f->tex = i.texture;
			f->anchor = { 0, 1 };
			f->spriteSize = { float(i.w), float(i.h) };
			f->textureRect = { float(i.x), float(i.y), f->spriteSize };
			auto& s = ss[idx];
			s.SetTexture(std::move(f));
			s.SetColor({ 255, 255, 255, 255 });
		}
	}

	// init camera for pixel 1:1 display
	auto nCols = uint32_t(w / map.tileWidth);
	auto nRows = uint32_t(nCols * (h / w));
	cam.SetRange(this, map, { 0, 0, float(nCols), (float)nRows });
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;

	auto now = xx::NowSteadyEpochSeconds();
	if (now - secs > 0.01) {
		secs = now;
		auto& r = cam.gridRect;
		if ((Pressed(KbdKeys::Up) || Pressed(KbdKeys::W)) && r.y > 0) {
			cam.SetPos({ r.x, r.y - 1 });
		}
		if ((Pressed(KbdKeys::Down) || Pressed(KbdKeys::S)) && r.y + r.h + 1 < map.height) {
			cam.SetPos({r.x, r.y + 1});
		}
		if ((Pressed(KbdKeys::Left) || Pressed(KbdKeys::A)) && r.x > 0) {
			cam.SetPos({r.x - 1, r.y});
		}
		if ((Pressed(KbdKeys::Right) || Pressed(KbdKeys::D)) && r.x + r.w + 1 < map.width) {
			cam.SetPos({r.x + 1, r.y});
		}
		if (Pressed(KbdKeys::Z) && r.w > 1) {
			cam.SetRange(this, map, { r.x, r.y, r.w - 1, (r.w - 1) * (h / w) });
		}
		if (Pressed(KbdKeys::X) && r.w + r.x + 1 < map.width) {
			cam.SetRange(this, map, { r.x, r.y, r.w + 1, (r.w + 1) * (h / w) });
		}
	}

	for (uint32_t y = 0, ye = cam.gridRect.h; y < ye; ++y) {
		for (uint32_t x = 0, xe = cam.gridRect.w; x < xe; ++x) {
			auto& s = ss[(y + cam.gridRect.y) * map.width + cam.gridRect.x + x];
			s.SetScale(cam.spriteScale);
			s.SetPositon({ x * cam.spriteSize.w - w / 2 , h / 2 - y * cam.spriteSize.h });
			s.Draw(this);
		}
	}

	// display draw call
	lbCount.SetText(fnt1, xx::ToString("draw call = ", GetDrawCall(), ", quad count = ", GetDrawQuads()));
	lbCount.Draw(this);
	return 0;
}
