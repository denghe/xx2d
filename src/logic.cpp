﻿#include "pch.h"
#include "logic.h"

void Logic::Init() {
	rnd.SetSeed();

	auto t1 = eg.TextureCacheLoad("res/zazaka.pkm"sv);
	auto t2 = eg.TextureCacheLoad("res/mouse.pkm"sv);

	size_t numSprites = 500'000;// 1'000'000;

	ss.resize(numSprites);
	for (size_t i = 0; i < numSprites; i++) {
		auto& s = ss[i];
		//s.SetTexture(/*rnd.Get()*/i % 2 == 0 ? t1 : t2);
		s.SetTexture(t1);
		s.SetScale({ 1, 1 });
		auto c = rnd.Get(); auto cp = (uint8_t*)&c;
		s.SetColor({ cp[0], cp[1], cp[2], 255 });
		s.SetPositon({ float(rnd.Next(eg.w) - eg.w / 2), float(rnd.Next(eg.h) - eg.h / 2) });
	}
}

void Logic::Update(float delta) {
	int n = (int)ss.size();
#ifdef OPEN_MP_NUM_THREADS
#pragma omp parallel for
#endif
	for (int i = 0; i < n; ++i) {
		auto& s = ss[i];
		s.SetPositon({ float(rnd.Next(eg.w) - eg.w / 2), float(rnd.Next(eg.h) - eg.h / 2) });
	}

	// todo: sort by y example
	for (auto& s : ss) {
		eg.AutoBatchDrawQuad(*s.tex, s.verts);
	}
}
