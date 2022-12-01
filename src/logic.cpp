#include "pch.h"
#include "logic.h"

void Logic::Init() {
	rnd.SetSeed();

	// 已知问题: 由于自动合批所用 buffer 较大 ( 1万多个元素 ), 遇到这种只画 1 次就要切换的情况，就对显卡造成了巨量无效提交。 
	// 一个办法是改小 buffer. 一个办法是先 memcpy 到某处，切批时再按实际提交到显存，这样多一次 copy. 还有个办法就是上 贴图数组
	autoBatchSize = 50;

	auto t1 = xx::Make<Texture>(LoadTexture("res/zazaka.pkm"));
	auto t2 = xx::Make<Texture>(LoadTexture("res/mouse.pkm"));

	size_t numSprites = 10000;// 1'000'000;

	ss.resize(numSprites);
	for (size_t i = 0; i < numSprites; i++) {
		auto& s = ss[i];
		s.SetTexture(/*rnd.Get()*/i % 2 == 0 ? t1 : t2);
		s.SetScale({ 0.5, 0.5 });
		auto c = rnd.Get(); auto cp = (uint8_t*)&c;
		s.SetColor({ cp[0], cp[1], cp[2], 255 });
		s.SetPositon({ float(rnd.Next(w) - w / 2), float(rnd.Next(h) - h / 2) });
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

	// todo: 根据 节点关系，或 y 值之类的改变绘制顺序
	for (auto& s : ss) {
		AutoBatchDrawQuad(*s.tex, s.verts);
	}
}
