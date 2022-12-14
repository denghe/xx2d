#include "pch.h"
#include "logic.h"

void Logic::Init() {
	rnd.SetSeed();

	size_t numSprites = 50'000;

	auto t1 = TextureCacheLoad("res/zazaka.pkm"sv);
	auto t2 = TextureCacheLoad("res/mouse.pkm"sv);
	BMFont bmf;
	bmf.Load(this, "res/font2/basechars.fnt"sv);

	objs.resize(numSprites);

	for (size_t i = 0; i < numSprites; i++) {
		auto& s = objs[i].first;
		//s.SetTexture(/*rnd.Get()*/i % 2 == 0 ? t1 : t2);
		s.SetTexture(t1);
		s.SetScale({ 1, 1 });
		auto c = rnd.Get(); auto cp = (uint8_t*)&c;
		s.SetColor({ cp[0], cp[1], cp[2], 255 });
		s.SetPositon({ float(rnd.Next(w) - w / 2), float(rnd.Next(h) - h / 2) });

		auto& l = objs[i].second;
		l.SetText(bmf, rnd.NextWord(), 16);
		l.SetColor({ 255, 255, 255, 255 });
		l.SetPositon({ 0, 0 });
		l.SetAnchor({ 0.5, 0.5 });
	}
}

void Logic::Update(float delta) {
	for (auto& o : objs) {
		o.first.SetPositon({ float(rnd.Next(w) - w / 2), float(rnd.Next(h) - h / 2) });
		o.first.Draw(this);
	}
	for (auto& o : objs) {
		o.second.SetPositon({ o.first.lastPos.x, o.first.lastPos.y + o.first.TH() / 2 });
		o.second.SetAnchor({ 0.5, 0.5 });
		o.second.Draw(this);
	}
}
