#include "main.h"
#include "s20_xxmv.h"

namespace XxmvTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "XxmvTest::Scene::Init" << std::endl;

		xx::Shader_Yuva2Rgba::Init();

		auto [d, f] = xx::engine.LoadFileData("res/st_k100.xxmv");

		int r = mv.Load(d);
		assert(!r);

		auto&& shader = xx::engine.sm.GetShader<xx::Shader_Yuva2Rgba>();

		auto secs = xx::NowEpochSeconds();
		mv.ForeachFrame([&](int const& frameIndex, uint32_t const& w, uint32_t const& h
		, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData, uint32_t const& yaStride, uint32_t const& uvStride)->int {

			auto tex = xx::FrameBuffer().Init().Draw({ w, h }, true, {}, [&]() {
				shader.Draw(yData, uData, vData, aData, yaStride, uvStride, w, h, {});
			});

			texs.push_back(tex);

			return 0;
		});
		xx::CoutN("convert res/st_k100.xxmv all frames to texs. elapsed secs = ", xx::NowEpochSeconds(secs));

		spr.SetTexture(texs[cursor]);
	}

	int Scene::Update() {
		timePool += xx::engine.delta;
		while (timePool >= 1.f / 60) {
			timePool -= 1.f / 60;

			if (++cursor == texs.size()) {
				cursor = 0;
			}
			spr.SetTexture(texs[cursor]);
		}
		spr.Draw();
		return 0;
	}
}
