#include "pch.h"
#include "logic.h"

void Logic::Init() {

	rnd.SetSeed();
	//auto t1 = LoadTextureFromCache("res/zazaka.pkm"sv);
	auto t2 = LoadTextureFromCache("res/mouse.pkm"sv);
	auto fnt = LoadBMFont("res/font2/basechars.fnt"sv);
	LoadFramesFromCache(LoadTPData("res/aaa.plist"sv));		// key = zazaka.pkm
	LoadFramesFromCache(LoadTPData("res/bomb.plist"sv));	// key = bomb1.png ~ bomb36.png

	size_t numSprites = 50'000;
	objs.resize(numSprites);

	for (size_t i = 0; i < numSprites; i++) {
		auto& s = objs[i].first;
		if (i % 2) {
			s.SetTexture(frameCache.find("zazaka.pkm"sv)->second);
		}
		else {
			s.SetTexture(t2);
		}
		auto c = rnd.Get(); auto cp = (uint8_t*)&c;
		s.SetColor({ cp[0], cp[1], cp[2], 255 });
		s.SetPositon({ float(rnd.Next(w) - w / 2), float(rnd.Next(h) - h / 2) });

		auto& l = objs[i].second;
		l.SetText(fnt, rnd.NextWord(), 16);
		l.SetColor({ 255, 255, 255, 255 });
		l.SetPositon({ 0, 0 });
		l.SetAnchor({ 0.5, 0.5 });
	}

	//std::string key;
	//for (size_t j = 0; j < 10000; j++) {
	//	for (int i = 1; i <= 36; ++i) {
	//		auto&& [s, l] = objs.emplace_back();
	//		xx::Append(key, "bomb", i, ".png");
	//		s.SetTexture( frameCache[key] );
	//		s.SetScale({0.5, 0.5});
	//		key.clear();
	//		s.SetPositon({ float(rnd.Next(w) - w / 2), float(rnd.Next(h) - h / 2) });
	//	}
	//}

	//{
	//	auto&& o = objs.emplace_back();
	//	auto& s = o.first;
	//	s.SetTexture(frameCache.find("zazaka.pkm"sv)->second);
	//	s.SetScale({ 16, 16 });
	//	s.SetAnchorFromFrame();
	//}
}

void Logic::Update(float delta) {
	for (auto& o : objs) {
		o.first.SetPositon({ float(rnd.Next(w) - w / 2), float(rnd.Next(h) - h / 2) });
		o.first.Draw(this);
	}
	for (auto& o : objs) {
		o.second.SetPositon({ o.first.pos.x, o.first.pos.y + o.first.frame->spriteSize.h * o.first.anchor.y + o.first.frame->spriteSize.h });
		o.second.SetAnchor({ 0.5, 0.5 });
		o.second.Draw(this);
	}

	//for (auto& o : objs) {
	//	o.first.Draw(this);
	//}
}
