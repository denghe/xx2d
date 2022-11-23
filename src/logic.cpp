#include "pch.h"
#include "logic.h"

void Logic::Init() {
	t = xx::Make<Texture>(LoadTexture("res/zazaka.pkm"));
	size_t numSprites = 200000;
	ss.resize(numSprites);
	for (size_t i = 0; i < numSprites; i++) {
		auto& s = ss[i];
		s.SetTexture(t);
		s.SetScale({ 1, 1 });
		s.SetPositon({ float((rand() % w) - w/2) , float((rand() % h) - h/2) });
		s.SetColor({ uint8_t(rand() % 256), uint8_t(rand() % 256), uint8_t(rand() % 256), 255});
	}
}

void Logic::Update(float delta) {
	assert(w >= 0 && h >= 0);
	glViewport(0, 0, w, h);																									CheckGLError();
	glClear(GL_COLOR_BUFFER_BIT);																							CheckGLError();

#ifdef OPEN_MP_NUM_THREADS
	int n = (int)ss.size();
#pragma omp parallel for
#endif
	for (int i = 0; i < n; ++i) {
		auto& s = ss[i];
		s.SetPositon({ float((rand() % w) - w / 2) , float((rand() % h) - h / 2) });
	}

	auto numBatchs = n / batchSize;
	for (int j = 0; j < numBatchs; ++j) {
		DrawBatch(&ss[j * batchSize], batchSize);
	}
	if (auto left = n - numBatchs * batchSize) {
		DrawBatch(&ss[n - left], left);
	}
}
