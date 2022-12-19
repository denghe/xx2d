#include "pch.h"
#include "logic.h"

void Logic::Init() {
	rnd.SetSeed();
	fnt1 = LoadBMFont("res/font1/basechars.fnt"sv);
	fnt2 = LoadBMFont("res/font2/basechars.fnt"sv);
	LoadFramesFromCache(LoadTPData("res/zm.plist"sv));		// key = zazaka.pkm, mouse.pkm
	fZazaka = frameCache.find("zazaka.pkm"sv)->second;
	fMouse = frameCache.find("mouse.pkm"sv)->second;
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
			for (int i = 0; i < 100; ++i) {
				auto m = xx::Make<Monster>();
				auto&& [_, ok] = monsters.emplace(m);
				m->id = ++objsCounter;
				m->hp = 123;
				m->x = mousePosition.x;
				m->y = mousePosition.y;
				m->drawOrder = (double)m->y - m->id / 1000000000.;
				auto& s = m->sprite;
				auto& l = m->label;

				if (m->id % 2) {
					s.SetTexture(fZazaka);
					s.SetScale(rnd.Next(0.5f, 2.f));
				}
				else {
					s.SetTexture(fMouse);
					s.SetScale(rnd.Next(1.f, 4.f));
				}
				auto c = rnd.Get(); auto cp = (uint8_t*)&c;
				s.SetColor({ cp[0], cp[1], cp[2], 255 });
				s.SetPositon({ (float)m->x, (float)m->y });	// todo: mapping?

				l.SetText(fnt2, xx::ToString("id = ", m->id), 16);
				l.SetPositon({ s.pos.x, s.pos.y + s.frame->spriteSize.h * s.scale.y * (1 - s.anchor.y) });
				l.SetAnchor({ 0.5, 0 });
				l.scale = s.scale;

				coros.Add([](Logic* self, auto m)->CoType {
					CoYield;
					auto& s = m->sprite;
					auto& l = m->label;
					while (s.scale.x > 0.05) {
						s.SetScale(s.scale * 0.99);
						l.SetScale(s.scale);
						l.SetPositon({ s.pos.x, s.pos.y + s.frame->spriteSize.h * s.scale.y * (1 - s.anchor.y) });
						CoYield;
					}
					self->monsters.erase(m);
				}(this, m));
			}
		}
	}

	for (auto& o : monsters) {
		tmp.emplace_back(&*o);
	}
	std::sort(tmp.begin(), tmp.end(), [](auto const& a, auto const& b)->bool const { return a->drawOrder > b->drawOrder; });
	for (auto&& o : tmp) {
		o->sprite.Draw(this);
	}
	for (auto&& o : tmp) {
		o->label.Draw(this);
	}
	tmp.clear();

	lbCount.SetText(fnt1, xx::ToString("draw call = ", GetDrawCall(), ", quad count = ", GetDrawQuads()));
	lbCount.Draw(this);
	return 0;
}
