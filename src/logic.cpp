#include "pch.h"
#include "logic.h"

Frame_Duration const& Sprite_FrameAnim::GetCurrentFrame_Duration() const {
	return (*frameAnim)[cursor];
}

xx::Shared<Frame> const& Sprite_FrameAnim::GetCurrentFrame() const {
	return (*frameAnim)[cursor].frame;
}

void Sprite_FrameAnim::Step() {
	if (++cursor == frameAnim->size()) {
		cursor = 0;
	}
}

bool Sprite_FrameAnim::Update(float const& delta) {
	if (!frameAnim) return false;
	auto bak = cursor;
	timePool += delta;
LabBegin:
	auto fd = GetCurrentFrame_Duration();
	if (timePool >= fd.durationSeconds) {
		timePool -= fd.durationSeconds;
		Step();
		goto LabBegin;
	}
	return bak != cursor;
}

void Logic::Init() {
	// for display drawcall
	fnt1 = LoadBMFont("res/font1/basechars.fnt"sv);
	lbCount.SetPositon(ninePoints[1] + XY{ 10, 10 });
	lbCount.SetAnchor({0, 0});

	// load map
	TMX::FillTo(map, this, "res/tiledmap1/m1.tmx");

	// fill frames
	gidFrames.resize(map.gidInfos.size());
	for(size_t gid = 1, siz = map.gidInfos.size(); gid < siz; ++gid) {
		auto& i = map.gidInfos[gid];
		auto& f = *gidFrames[gid].Emplace();
		f.tex = i.image->texture;
		f.anchor = { 0, 1 };
		f.spriteSize = { (float)i.w, (float)i.h };
		f.textureRect = { (float)i.u, (float)i.v, (float)i.w, (float)i.h };
	}

	// fill anims
	gidFrameAnims.resize(map.gidInfos.size());
	for (size_t gid = 1, siz = map.gidInfos.size(); gid < siz; ++gid) {
		auto& i = map.gidInfos[gid];
		if (i.tile) {
			auto& tas = i.tile->animation;
			if (auto&& numAnims = tas.size()) {
				auto& a = *gidFrameAnims[gid].Emplace();
				a.resize(numAnims);
				for (size_t x = 0; x < numAnims; ++x) {
					a[x].frame = gidFrames[tas[x].gid];
					a[x].durationSeconds = tas[x].duration / 1000.f;
				}
			}
		}
	}

	// recursive scan & find out all tile layer & gen sprites
	std::vector<TMX::Layer_Tile*> lts;
	TMX::Fill(lts, map.layers);
	for (auto& lt : lts) {
		std::vector<Sprite_FrameAnim> sfas;
		sfas.resize(lt->gids.size());
		for (int cy = 0; cy < (int)map.height; ++cy) {
			for (int cx = 0; cx < (int)map.width; ++cx) {
				auto&& idx = cy * (int)map.width + cx;
				auto&& gid = lt->gids[idx];
				if (!gid) continue;

				auto& sfa = sfas[idx];
				auto&& s = *sfa.sprite.Emplace();
				if (sfa.frameAnim = gidFrameAnims[gid]) {
					s.SetTexture(sfa.GetCurrentFrame());
					allSfas.push_back(&sfa);
				} else {
					s.SetTexture(gidFrames[gid]);
				}
				s.SetColor({ 255, 255, 255, 255 });
				s.SetScale({ 1, 1 });
				s.SetPositon({ float(cx * (int)map.tileWidth), float(-cy * (int)map.tileHeight) });
				s.Commit();
			}
		}
		tileLayer_Sprites.emplace(lt, std::move(sfas));
	}

	// init camera
	cam.Init({w, h}, map);

	secs = xx::NowSteadyEpochSeconds();
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;

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

	// update all anims
	auto delta = xx::NowSteadyEpochSeconds(secs);
	for (auto&& sfa : allSfas) {
		if (sfa->Update(delta)) {
			sfa->sprite->SetTexture(sfa->GetCurrentFrame());
			sfa->sprite->Commit();
		}
	}

	// draw screen range sprites
	for (auto& [layer, sfas] : tileLayer_Sprites) {
		for (uint32_t y = cam.rowFrom; y < cam.rowTo; ++y) {
			for (uint32_t x = cam.columnFrom; x < cam.columnTo; ++x) {
				auto&& idx = y * cam.worldColumnCount + x;
				auto&& sfa = sfas[idx];
				if (!sfa.sprite) continue;

				//if (sfa.Update(delta)) {
				//	sfa.sprite->SetTexture(sfa.GetCurrentFrame());
				//	sfa.sprite->Commit();
				//}
				sfa.sprite->Draw(this, cam);
			}
		}
	}

	// display draw call
	lbCount.SetText(fnt1, std::format("draw call = {}, quad count = {}, cam.pos = {},{}", GetDrawCall(), GetDrawQuads(), cam.pos.x, cam.pos.y ));
	lbCount.Commit();
	lbCount.Draw(this);
	return 0;
}
