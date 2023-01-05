#include "pch.h"
#include "logic.h"

void TmxCamera::Init(Size const& screenSize, TMX::Map& map) {
	tileWidth = map.tileWidth;
	tileHeight = map.tileHeight;

	worldRowCount = map.height;
	worldColumnCount = map.width;

	worldPixel.w = tileWidth * worldColumnCount;
	worldPixel.h = tileHeight * worldRowCount;

	this->screenSize = screenSize;

	Commit();
}

void TmxCamera::Commit() {
	if (!dirty) return;
	dirty = false;

	// todo: scale

	auto halfNumRows = screenSize.h / tileHeight / 2;
	int32_t posRowIndex = pos.y / tileWidth;
	rowFrom = posRowIndex - halfNumRows;
	rowTo = posRowIndex + halfNumRows + 2;
	if (rowFrom < 0) {
		rowFrom = 0;
	}
	if (rowTo > worldRowCount) {
		rowTo = worldRowCount;
	}

	auto halfNumColumns = screenSize.w / tileWidth / 2;
	int32_t posColumnIndex = pos.x / tileHeight;
	columnFrom = posColumnIndex - halfNumColumns;
	columnTo = posColumnIndex + halfNumColumns + 1;
	if (columnFrom < 0) {
		columnFrom = 0;
	}
	if (columnTo > worldColumnCount) {
		columnTo = worldColumnCount;
	}

	offset = { -pos.x, pos.y };
}

void TmxCamera::SetScale(float const& scale) {
	this->scale = { scale, scale };
	dirty = true;
}

void TmxCamera::SetScreenSize(Size const& wh) {
	this->screenSize = wh;
	dirty = true;
}

void TmxCamera::SetPos(XY const& xy) {
	this->pos = xy;
	dirty = true;
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
	for (int cy = 0; cy < (int)map.height; ++cy) {
		for (int cx = 0; cx < (int)map.width; ++cx) {
			auto&& idx = cy * (int)map.width + cx;
			auto&& gid = lt.gids[idx];
			assert(gid < map.gidInfos.size());
			auto&& i = map.gidInfos[gid];
			assert(i.image);
			auto f = xx::Make<Frame>();
			f->tex = i.image->texture;
			f->anchor = { 0, 1 };
			if (i.IsSingleImage()) {
				f->spriteSize = { float(i.image->width), float(i.image->height) };
				f->textureRect = { 0, 0, f->spriteSize };
			} else {
				f->spriteSize = { float(i.w), float(i.h) };
				f->textureRect = { float(i.x), float(i.y), f->spriteSize };
			}
			auto& s = ss[idx];
			s.SetTexture(std::move(f));
			s.SetColor({ 255, 255, 255, 255 });
			s.SetScale({ 1, 1 });
			s.SetPositon({ float(cx * i.w), float(-cy * i.h) });
			s.Commit();
		}
	}

	// init camera
	cam.Init({w, h}, map);
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;

	auto now = xx::NowSteadyEpochSeconds();
	if (now - secs > 0.001) {
		secs = now;
		if ((Pressed(KbdKeys::Up) || Pressed(KbdKeys::W)) && cam.pos.y > 0) {
			cam.SetPos({ cam.pos.x, cam.pos.y - 1 });
		}
		if ((Pressed(KbdKeys::Down) || Pressed(KbdKeys::S)) && cam.pos.y + 1 < cam.worldPixel.h) {
			cam.SetPos({ cam.pos.x, cam.pos.y + 1 });
		}
		if ((Pressed(KbdKeys::Left) || Pressed(KbdKeys::A)) && cam.pos.x > 0) {
			cam.SetPos({ cam.pos.x - 1, cam.pos.y });
		}
		if ((Pressed(KbdKeys::Right) || Pressed(KbdKeys::D)) && cam.pos.x + 1 < cam.worldPixel.w) {
			cam.SetPos({ cam.pos.x + 1, cam.pos.y });
		}
		if (Pressed(KbdKeys::Z) && cam.scale.x < 100) {
			cam.SetScale(cam.scale.x + 0.01);
		}
		if (Pressed(KbdKeys::X) && cam.scale.x > 0.01) {
			cam.SetScale(cam.scale.x - 0.01);
		}
		cam.Commit();
	}

	for (uint32_t y = cam.rowFrom; y < cam.rowTo; ++y) {
		for (uint32_t x = cam.columnFrom; x < cam.columnTo; ++x) {
			auto&& s = ss[y * cam.worldColumnCount + x];
			s.Draw(this, cam);
		}
	}

	// display draw call
	lbCount.SetText(fnt1, std::format("draw call = {}, quad count = {}, cam.pos = {},{}", GetDrawCall(), GetDrawQuads(), cam.pos.x, cam.pos.y ));
	lbCount.Commit();
	lbCount.Draw(this);
	return 0;
}
