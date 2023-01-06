#include "pch.h"
#include "logic.h"

// known issue: sometimes no display anim	// todo: fix

Frame_Duration const& Anim::GetCurrentFrame_Duration() const {
	return fa[cursor];
}

xx::Shared<Frame> const& Anim::GetCurrentFrame() const {
	return fa[cursor].frame;
}

void Anim::Step() {
	if (++cursor == fa.size()) {
		cursor = 0;
	}
}

bool Anim::Update(float const& delta) {
	auto&& bak = cursor;
	timePool += delta;
LabBegin:
	auto&& fd = GetCurrentFrame_Duration();
	if (timePool >= fd.durationSeconds) {
		timePool -= fd.durationSeconds;
		Step();
		goto LabBegin;
	}
	return bak != cursor;
}


void Sprite_Anim::Draw(Engine* eg, TMX::Camera& cam) {
	if (anim) {
		if (auto&& f = anim->GetCurrentFrame(); sprite->frame != f) {
			sprite->SetTexture(f);
			sprite->Commit();
		}
	}
	sprite->Draw(eg, cam);
}


void Logic::Init() {
	// for display drawcall
	fnt1 = LoadBMFont("res/font1/basechars.fnt"sv);
	lbCount.SetPositon(ninePoints[1] + XY{ 10, 10 });
	lbCount.SetAnchor({0, 0});

	// load map
	TMX::FillTo(map, this, "res/tiledmap1/m1.tmx");

	// fill frames
	gidFAs.resize(map.gidInfos.size());
	for(size_t gid = 1, siz = map.gidInfos.size(); gid < siz; ++gid) {
		auto& i = map.gidInfos[gid];
		auto& f = *gidFAs[gid].frame.Emplace();
		f.tex = i.image->texture;
		f.anchor = { 0, 1 };
		f.spriteSize = { (float)i.w, (float)i.h };
		f.textureRect = { (float)i.u, (float)i.v, (float)i.w, (float)i.h };
	}

	// fill anims
	for (size_t gid = 1, siz = map.gidInfos.size(); gid < siz; ++gid) {
		auto& i = map.gidInfos[gid];
		if (i.tile) {
			auto& tas = i.tile->animation;
			if (auto&& numAnims = tas.size()) {
				auto& fa = gidFAs[gid].anim.Emplace()->fa;
				fa.resize(numAnims);
				for (size_t x = 0; x < numAnims; ++x) {
					fa[x].frame = gidFAs[tas[x].gid].frame;
					fa[x].durationSeconds = tas[x].duration / 1000.f;
				}
				anims.emplace_back(gidFAs[gid].anim);
			}
		}
	}

	// recursive find all tile layer & gen sprites
	std::vector<TMX::Layer_Tile*> lts;
	TMX::Fill(lts, map.layers);
	lsass.resize(lts.size());
	for(size_t i = 0, ie = lts.size(); i < ie; ++i) {
		auto&& lt = lts[i];
		auto&& lsas = lsass[i];
		auto&& sas = lsas.sas;
		lsas.layer = lt;
		lsas.sas.resize(lt->gids.size());
		for (int cy = 0; cy < (int)map.height; ++cy) {
			for (int cx = 0; cx < (int)map.width; ++cx) {
				auto&& idx = cy * (int)map.width + cx;
				auto&& gid = lt->gids[idx];
				if (!gid) continue;

				auto& sa = sas[idx];
				auto&& s = *sa.sprite.Emplace();
				if (sa.anim = gidFAs[gid].anim) {
					s.SetTexture(sa.anim->GetCurrentFrame());
				} else {
					s.SetTexture(gidFAs[gid].frame);
				}
				s.SetColor({ 255, 255, 255, 255 });
				s.SetScale({ 1, 1 });
				s.SetPositon({ float(cx * (int)map.tileWidth), float(-cy * (int)map.tileHeight) });
				s.Commit();
			}
		}
	}

	// init camera
	cam.Init({w, h}, map);

	// update for anim
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
	auto&& delta = xx::NowSteadyEpochSeconds(secs);
	for (auto&& a : anims) {
		a->Update(delta);
	}

	// draw screen range sprites
	for (auto& [layer, sas] : lsass) {
		for (uint32_t y = cam.rowFrom; y < cam.rowTo; ++y) {
			for (uint32_t x = cam.columnFrom; x < cam.columnTo; ++x) {
				auto&& idx = y * cam.worldColumnCount + x;
				auto&& sa = sas[idx];
				if (!sa.sprite) continue;
				sa.Draw(this, cam);
			}
		}
	}

	// display draw call
	lbCount.SetText(fnt1, std::format("draw call = {}, quad count = {}, cam.pos = {},{}", GetDrawCall(), GetDrawQuads(), cam.pos.x, cam.pos.y ));
	lbCount.Commit();
	lbCount.Draw(this);
	return 0;
}
