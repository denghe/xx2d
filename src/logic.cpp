#include "pch.h"
#include "logic.h"


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

	// init sprites
	TMX::Fill(ss, map, lt);

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

	cam.Draw(this, ss);

	// display draw call
	lbCount.SetText(fnt1, std::format("draw call = {}, quad count = {}, cam.pos = {},{}", GetDrawCall(), GetDrawQuads(), cam.pos.x, cam.pos.y ));
	lbCount.Commit();
	lbCount.Draw(this);
	return 0;
}
