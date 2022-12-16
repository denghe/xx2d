#include "pch.h"
#include "logic.h"

void Logic::Init() {

	rnd.SetSeed();
	auto t1 = LoadTextureFromCache("res/zazaka.pkm"sv);
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
			auto scale = 0.5f + rnd.Next<float>() * 1.5f;
			s.SetScale({ scale, scale });
		}
		else {
			s.SetTexture(t2);
			auto scale = 1.f + rnd.Next<float>() * 3.f;
			s.SetScale({ scale, scale });
		}
		auto c = rnd.Get(); auto cp = (uint8_t*)&c;
		s.SetColor({ cp[0], cp[1], cp[2], 255 });
		s.SetPositon({ float(rnd.Next(w) - w / 2), float(rnd.Next(h) - h / 2) });

		auto& l = objs[i].second;
		l.SetText(fnt, rnd.NextWord(), 16);
		l.SetAnchor({ 0.5, 0 });
		l.scale = s.scale;
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

	//auto&& o = objs.emplace_back().second;
	//o.SetText(fnt, "ofplo"sv, 512);
	//o.SetAnchor({ 0.5, 0 });
}

void Logic::Update(float delta) {
	for (auto& o : objs) {
		o.first.SetPositon({ float(rnd.Next(w) - w / 2), float(rnd.Next(h) - h / 2) });
		o.first.Draw(this);
	}
	for (auto& [s, l] : objs) {
		l.SetPositon({ s.pos.x, s.pos.y + s.frame->spriteSize.h * s.scale.y * ( 1 - s.anchor.y )});
		l.Draw(this);
	}

	//for (auto& o : objs) {
	//	o.first.Draw(this);
	//}

	//for (auto& o : objs) {
	//	o.second.Draw(this);
	//}
}
