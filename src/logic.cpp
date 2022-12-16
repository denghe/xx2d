#include "pch.h"
#include "logic.h"

void Logic::Init() {
	rnd.SetSeed();
	lastMouseClickTime = xx::NowSteadyEpochSeconds();
	t = LoadTextureFromCache("res/mouse.pkm"sv);
	fnt = LoadBMFont("res/font2/basechars.fnt"sv);
	LoadFramesFromCache(LoadTPData("res/aaa.plist"sv));		// key = zazaka.pkm
	lbCount.SetPositon(ninePoints[1] + XY{ 10, 10 });
	lbCount.SetAnchor({0, 0});
}

void Logic::Update(float delta) {
	if (mouseButtonStates[0]) {
		auto sec = xx::NowSteadyEpochSeconds();
		if (sec - lastMouseClickTime > 0.001) {
			lastMouseClickTime = sec;

			auto& [s, l] = objs.emplace_back();
			if (objs.size() % 2) {
				s.SetTexture(frameCache.find("zazaka.pkm"sv)->second);
				s.SetScale(rnd.Next(0.5f, 2.f));
			}
			else {
				s.SetTexture(t);
				s.SetScale(rnd.Next(1.f, 4.f));
			}
			auto c = rnd.Get(); auto cp = (uint8_t*)&c;
			s.SetColor({ cp[0], cp[1], cp[2], 255 });
			s.SetPositon(mousePosition);

			l.SetText(fnt, rnd.NextWord(), 16);
			l.SetPositon({ s.pos.x, s.pos.y + s.frame->spriteSize.h * s.scale.y * (1 - s.anchor.y) });
			l.SetAnchor({ 0.5, 0 });
			l.scale = s.scale;
		}
	}
	for (auto& o : objs) {
		o.first.Draw(this);
	}
	for (auto& [s, l] : objs) {
		l.Draw(this);
	}

	lbCount.SetText(fnt, xx::ToString("draw call = ", GetDrawCall(), ", obj count = ", objs.size()));
	lbCount.Draw(this);
}
















//LoadFramesFromCache(LoadTPData("res/bomb.plist"sv));	// key = bomb1.png ~ bomb36.png

//size_t numSprites = 50'000;
//objs.resize(numSprites);

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

	//for (auto& o : objs) {
	//	o.first.Draw(this);
	//}

	//for (auto& o : objs) {
	//	o.second.Draw(this);
	//}