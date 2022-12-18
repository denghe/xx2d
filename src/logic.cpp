#include "pch.h"
#include "logic.h"

void Logic::Init() {
	rnd.SetSeed();
	fnt1 = LoadBMFont("res/font1/basechars.fnt"sv);
	fnt2 = LoadBMFont("res/font2/basechars.fnt"sv);
	LoadFramesFromCache(LoadTPData("res/zm.plist"sv));		// key = zazaka.pkm, mouse.pkm
	fZazaka = frameCache.find("zazaka.pkm"sv)->second;
	fMouse = frameCache.find("mouse.pkm"sv)->second;
	t = LoadTextureFromCache("res/bomb.pkm"sv);
	lbCount.SetPositon(ninePoints[1] + XY{ 10, 10 });
	lbCount.SetAnchor({0, 0});
}

int Logic::Update() {
	if (Pressed(KbdKeys::Escape)) return 1;

	constexpr double fixedFrameDelta = 1.0 / 60;
	timePool += lastDelta;
	while (timePool >= fixedFrameDelta) {
		timePool -= fixedFrameDelta;
		coros();

		if (Pressed(Mbtns::Left)) {
			auto&& [iter, ok] = objs.emplace(++objsCounter, std::make_pair(Sprite{}, Label{}));
			assert(ok);
			auto& [s, l] = iter->second;

			if (objs.size() % 2) {
				s.SetTexture(fZazaka);
				s.SetScale(rnd.Next(0.5f, 2.f));
			}
			else {
				s.SetTexture(fMouse);
				s.SetScale(rnd.Next(1.f, 4.f));
			}
			auto c = rnd.Get(); auto cp = (uint8_t*)&c;
			s.SetColor({ cp[0], cp[1], cp[2], 255 });
			s.SetPositon(mousePosition);

			l.SetText(fnt2, rnd.NextWord(), 16);
			l.SetPositon({ s.pos.x, s.pos.y + s.frame->spriteSize.h * s.scale.y * (1 - s.anchor.y) });
			l.SetAnchor({ 0.5, 0 });
			l.scale = s.scale;

			coros.Add([](Logic* self, size_t k, Sprite& s, Label& l)->CoType {
				CoYield;
				while (s.scale.x > 0.05) {
					s.SetScale(s.scale * 0.99);
					l.SetScale(s.scale);
					l.SetPositon({ s.pos.x, s.pos.y + s.frame->spriteSize.h * s.scale.y * (1 - s.anchor.y) });
					CoYield;
				}
				self->objs.erase(k);
			}(this, objsCounter, s, l));
		}
	}

	for (auto& [k, v] : objs) {
		v.first.Draw(this);
	}
	for (auto& [k, v] : objs) {
		v.second.Draw(this);
	}
	lbCount.SetText(fnt1, xx::ToString("draw call = ", GetDrawCall(), ", obj count = ", objs.size()));
	lbCount.Draw(this);
	return 0;
}
