﻿#include "main.h"
#include "s1_tiledmap.h"

namespace TiledMap {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "TiledMap::Scene::Init" << std::endl;

		// load map
		xx::TMX::FillTo(map, "res/tiledmap2/m.tmx"sv);
		mapTileYOffset = map.tileHeight * mapTileLogicAnchorY;

		// fill frames
		gidFAs.resize(map.gidInfos.size());
		for (size_t gid = 1, siz = map.gidInfos.size(); gid < siz; ++gid) {
			auto& i = map.gidInfos[gid];
			auto& f = *gidFAs[gid].frame.Emplace();
			f.tex = i.image->texture;
			f.anchor = { 0, float(map.tileHeight) / i.h };
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

		// recursive find all tile layer
		std::vector<xx::TMX::Layer_Tile*> lts;
		xx::TMX::Fill(lts, map.layers);

		// locate & store
		layerBG.layer = xx::TMX::FindLayer(lts, "bg"sv);
		assert(layerBG.layer);
		layerTrees.layer = xx::TMX::FindLayer(lts, "trees"sv);
		assert(layerTrees.layer);

		// func for make sprites
		auto&& MakeSprites = [&](Layer_SAs& lsas) {
			lsas.sas.resize(lsas.layer->gids.size());
			for (int cy = 0; cy < (int)map.height; ++cy) {
				for (int cx = 0; cx < (int)map.width; ++cx) {
					auto&& idx = cy * (int)map.width + cx;
					auto&& gid = lsas.layer->gids[idx];
					if (!gid) continue;

					auto& sa = lsas.sas[idx];
					auto&& s = *sa.sprite.Emplace();
					if ((sa.anim = gidFAs[gid].anim)) {
						s.SetFrame(sa.anim->GetCurrentAnimFrame().frame);
					} else {
						s.SetFrame(gidFAs[gid].frame);
					}
					s.SetColor({ 255, 255, 255, 255 })
						.SetScale({ 1, 1 })
						.SetPosition({ float(cx * (int)map.tileWidth), float(-cy * (int)map.tileHeight) });
				}
			}
		};

		// make bg & others layer's sprites
		MakeSprites(layerBG);
		MakeSprites(layerTrees);

		// fill wall flags
		auto&& bgGids = layerBG.layer->gids;
		auto&& numGids = bgGids.size();
		walls.resize(numGids);
		for (size_t i = 0; i < numGids; ++i) {
			assert(bgGids[i]);
			if (auto&& tile = map.gidInfos[bgGids[i]].tile) {
				walls[i] = tile->class_ == "w"sv;
			}
		}

		// init camera
		cam.Init({ xx::engine.w, xx::engine.h }, map);
		cam.SetPosition({ 830, 510 });

		// load plist
		player.tp.Fill("res/ww.plist"sv);
		float maxSpriteHeight{}, anchor{ 0.5 };
		for (auto& f : player.tp.frames) {
			f->anchor = { anchor, anchor };
			player.anim.afs.push_back({ f, 1.f / 60 });
			if (f->spriteSize.y > maxSpriteHeight) {
				maxSpriteHeight = f->spriteSize.y;
			}
		}
		player.yOffset = maxSpriteHeight * player.scale * anchor;
		player.sprite.SetFrame(player.anim.GetCurrentAnimFrame().frame)
			.SetScale(player.scale);
		player.footPos = cam.pos;
	}

	int Scene::Update() {
		auto& eg = xx::engine;
		timePool += eg.delta;
		auto timePoolBak = timePool;
		while (timePool >= 1.f / 60) {
			timePool -= 1.f / 60;

			xx::XY camInc{ 10 / cam.scale.x, 10 / cam.scale.y };
			if ((eg.Pressed(xx::KbdKeys::Up))) {
				auto y = cam.pos.y - camInc.y;
				cam.SetPositionY(y < 0 ? 0 : y);
			}
			if ((eg.Pressed(xx::KbdKeys::Down))) {
				auto y = cam.pos.y + camInc.y;
				cam.SetPositionY(y >= cam.worldPixel.y ? (cam.worldPixel.y - std::numeric_limits<float>::epsilon()) : y);
			}
			if ((eg.Pressed(xx::KbdKeys::Left))) {
				auto x = cam.pos.x - camInc.x;
				cam.SetPositionX(x < 0 ? 0 : x);
			}
			if ((eg.Pressed(xx::KbdKeys::Right))) {
				auto x = cam.pos.x + camInc.x;
				cam.SetPositionX(x >= cam.worldPixel.x ? (cam.worldPixel.x - std::numeric_limits<float>::epsilon()) : x);
			}
			if (eg.Pressed(xx::KbdKeys::Z)) {
				auto x = cam.scale.x + 0.001f;
				cam.SetScale(x < 100 ? x : 100);
			}
			if (eg.Pressed(xx::KbdKeys::X)) {
				auto x = cam.scale.x - 0.001f;
				cam.SetScale(x > 0.001 ? x : 0.001);
			}
			cam.Commit();

			xx::XY playerInc{ 8 * player.sprite.scale.x, 8 * player.sprite.scale.y };
			if (eg.Pressed(xx::KbdKeys::W)) {
				auto y = player.footPos.y - playerInc.y;
				player.footPos.y = y < 0 ? 0 : y;
				player.dirty = true;
			}
			if (eg.Pressed(xx::KbdKeys::S)) {
				auto y = player.footPos.y + playerInc.y;
				player.footPos.y = y >= cam.worldPixel.y ? (cam.worldPixel.y - std::numeric_limits<float>::epsilon()) : y;
				player.dirty = true;
			}
			if (eg.Pressed(xx::KbdKeys::A)) {
				auto x = player.footPos.x - playerInc.x;
				player.footPos.x = x < 0 ? 0 : x;
				player.dirty = true;
				player.sprite.SetFlipX(false);
			}
			if (eg.Pressed(xx::KbdKeys::D)) {
				auto x = player.footPos.x + playerInc.x;
				player.footPos.x = x >= cam.worldPixel.x ? (cam.worldPixel.x - std::numeric_limits<float>::epsilon()) : x;
				player.dirty = true;
				player.sprite.SetFlipX(true);
			}

		}

		// if delta >= 1.f / 60 then elapsedSecs > 0, mean need update anims
		auto elapsedSecs = timePoolBak - timePool;

		// update player
		if (player.dirty) {
			player.dirty = false;
			player.sprite.SetPosition({ player.footPos.x, player.yOffset - player.footPos.y });
			if (elapsedSecs > 0) {
				if (player.anim.Update(elapsedSecs)) {
					player.sprite.SetFrame(player.anim.GetCurrentAnimFrame().frame);
				}
			}
		}

		// update all anims
		if (elapsedSecs > 0) {
			for (auto&& a : anims) {
				a->Update(elapsedSecs);
			}
		}

		// draw screen range layer's sprites
		auto&& DrawLayerSprites = [&](std::vector<Sprite_Anim>& sas, int32_t const& rowFrom, int32_t const& rowTo) {
			for (auto y = rowFrom; y < rowTo; ++y) {
				for (auto x = cam.columnFrom; x < cam.columnTo; ++x) {
					auto&& idx = y * cam.worldColumnCount + x;
					if (auto&& sa = sas[idx]; sa.sprite) {
						// update frame by anim
						if (sa.anim) {
							if (auto&& f = sa.anim->GetCurrentAnimFrame().frame; sa.sprite->frame != f) {
								sa.sprite->SetFrame(f);
							}
						}
						sa.sprite->Draw(cam.at);
					}
				}
			}
		};

		// draw bg first
		DrawLayerSprites(layerBG.sas, cam.rowFrom, cam.rowTo);

		// draw above player rows
		int32_t playerRowIdx = (player.footPos.y + mapTileYOffset) / cam.tileHeight;
		DrawLayerSprites(layerTrees.sas, cam.rowFrom, playerRowIdx);

		// draw player( override above rows )
		player.sprite.Draw(cam.at);

		// draw after player rows
		DrawLayerSprites(layerTrees.sas, playerRowIdx, cam.rowTo);

		return 0;
	}

}
