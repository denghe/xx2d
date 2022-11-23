#include "pch.h"
#include "logic.h"

void Logic::Init() {
	rnd.SetSeed();

	auto t = xx::Make<Texture>(LoadTexture("res/zazaka.pkm"));
	size_t numSprites = 1'000'000;
	ss.resize(numSprites);
	for (size_t i = 0; i < numSprites; i++) {
		auto& s = ss[i];
		s.SetTexture(t);
		s.SetScale({ 0.5, 0.5 });	// 缩小到 0.5 时，100万对象，fps 从 37 提升到 79. 继续缩小并没什么收益了
		s.SetPositon({ float(rnd.Next(w) - w/2), float(rnd.Next(h) - h/2) });
		auto c = rnd.Get(); auto cp = (uint8_t*)&c;
		s.SetColor({ cp[0], cp[1], cp[2], 255});
	}
}

void Logic::Update(float delta) {
	int n = (int)ss.size();
#ifdef OPEN_MP_NUM_THREADS
#pragma omp parallel for
#endif
	for (int i = 0; i < n; ++i) {
		auto& s = ss[i];
		s.SetPositon({ float(rnd.Next(w) - w / 2), float(rnd.Next(h) - h / 2) });
	}

	DrawSprites(ss.data(), ss.size());
}
