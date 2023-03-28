#include "main.h"
#include "s20_xxmv.h"

namespace XxmvTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "XxmvTest::Scene::Init" << std::endl;

		auto [d, f] = xx::engine.LoadFileData("res/st_k100.xxmv");

		int r = mv.Load(d);
		assert(!r);

		r = mv.ForeachFrame([&](int const& frameIndex, uint32_t const& w, uint32_t const& h, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData, uint32_t const& yaStride, uint32_t const& uvStride)->int {
			// todo
			std::cout << frameIndex << std::endl;
			return 0;
		});
		assert(!r);

		// todo
	}

	int Scene::Update() {

		return 0;
	}
}
