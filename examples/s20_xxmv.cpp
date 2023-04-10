#include "main.h"
#include "s20_xxmv.h"

namespace XxmvTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "XxmvTest::Scene::Init" << std::endl;

		auto [d, f] = xx::engine.LoadFileData("res/st_m_50.webm");

		int r = mv.Load(d);
		if (r) {
			std::cout << "load error. r = " << r << std::endl;
		}

		auto&& shader = xx::engine.sm.GetShader<xx::Shader_Yuva2Rgba>();
		int c = 0;

		auto secs = xx::NowEpochSeconds();
		mv.ForeachFrame([&](int const& frameIndex, uint32_t const& w, uint32_t const& h
		, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData, uint32_t const& yaStride, uint32_t const& uvStride)->int {

			++c;
			return 0;
		});
		xx::CoutN("calc ", f, " all frames. numFrames = ", texs.size() , ". c = ", c, ".  elapsed secs = ", xx::NowEpochSeconds(secs));
		
		mv.ForeachFrame([&](int const& frameIndex, uint32_t const& w, uint32_t const& h
		, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData, uint32_t const& yaStride, uint32_t const& uvStride)->int {

			auto tex = xx::FrameBuffer().Init().Draw({ w, h }, true, xx::RGBA8{}, [&]() {
				shader.Draw(yData, uData, vData, aData, yaStride, uvStride, w, h, {});
			});

			texs.push_back(tex);

			return 0;
		});
		xx::CoutN("convert ", f, " all frames to texs. numFrames = ", texs.size() , " elapsed secs = ", xx::NowEpochSeconds(secs));

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
