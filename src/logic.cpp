#include "pch.h"
#include "logic.h"

void Logic::Init() {
	// for display drawcall
	fnt1 = LoadBMFont("res/font1/basechars.fnt"sv);
	lbCount.SetPositon(ninePoints[1] + XY{ 10, 10 });
	lbCount.SetAnchor({0, 0});

	// load plist
	tp.Fill(this, "res/ww.plist"sv);

	// load map
	TMX::FillTo(map, this, "res/tiledmap1/m1.tmx"sv);

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
				auto& afs = gidFAs[gid].anim.Emplace()->afs;
				afs.resize(numAnims);
				for (size_t x = 0; x < numAnims; ++x) {
					afs[x].frame = gidFAs[tas[x].gid].frame;
					afs[x].durationSeconds = tas[x].duration / 1000.f;
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
					s.SetTexture(sa.anim->GetCurrentAnimFrame().frame);
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

	XY inc{ 1 / cam.scale.x, 1 / cam.scale.y };
	if ((Pressed(KbdKeys::Up) || Pressed(KbdKeys::W))) {
		auto y = cam.pos.y - inc.y;
		cam.SetPosY(y < 0 ? 0 : y);
	}
	if ((Pressed(KbdKeys::Down) || Pressed(KbdKeys::S))) {
		auto y = cam.pos.y + inc.y;
		cam.SetPosY(y >= cam.worldPixel.h ? (cam.worldPixel.h - std::numeric_limits<float>::epsilon()) : y);
	}
	if ((Pressed(KbdKeys::Left) || Pressed(KbdKeys::A))) {
		auto x = cam.pos.x - inc.x;
		cam.SetPosX(x < 0 ? 0 : x);
	}
	if ((Pressed(KbdKeys::Right) || Pressed(KbdKeys::D))) {
		auto x = cam.pos.x + inc.x;
		cam.SetPosX(x >= cam.worldPixel.w ? (cam.worldPixel.w - std::numeric_limits<float>::epsilon()) : x);
	}
	if (Pressed(KbdKeys::Z)) {
		auto x = cam.scale.x + 0.001f;
		cam.SetScale(x < 100 ? x : 100);
	}
	if (Pressed(KbdKeys::X)) {
		auto x = cam.scale.x - 0.001f;
		cam.SetScale(x > 0.001 ? x : 0.001);
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

				// update frame by anim
				if (sa.anim) {
					if (auto&& f = sa.anim->GetCurrentAnimFrame().frame; sa.sprite->frame != f) {
						sa.sprite->SetTexture(f);
						sa.sprite->Commit();
					}
				}
				sa.sprite->Draw(this, cam);
			}
		}
	}

	// display draw call
	lbCount.SetText(fnt1, xx::ToString("draw call = ", GetDrawCall(), ", quad count = ", GetDrawQuads(), ", cam.scale = ", cam.scale.x, ", cam.pos = ", cam.pos.x, ",", cam.pos.y));
	lbCount.Commit();
	lbCount.Draw(this);
	return 0;
}
