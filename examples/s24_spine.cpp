#include "main.h"
#include "s24_spine.h"

namespace SpineTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;
		std::cout << "SpineTest::Scene::Init" << std::endl;

		auto skelFN = xx::engine.GetFullPath("spine-runtimes/spine-sfml/cpp/data/spineboy-pro.skel");
		auto atlasFN = xx::engine.GetFullPath("spine-runtimes/spine-sfml/cpp/data/spineboy-pma.atlas");
		spShared.Init1(atlasFN.data()).Init2_Skel(skelFN.data(), 0.5);

		for (int i = 0; i < 1000; i++) {
			tasks.AddTask([](Scene* self, int delay)->xx::Task<> {
				while (delay-- >= 0) co_yield 0;
				spine::XxPlayer sp;
				sp.Init(self->spShared)
					.SetMix("walk", "jump", 0.2f)
					.SetMix("jump", "run", 0.2f)
					.SetPosition(xx::engine.rnd.Next<float>(-700, 700), xx::engine.rnd.Next<float>(-400, 100))
					.AddAnimation(0, "walk", true, 0)
					.AddAnimation(0, "jump", false, 3)
					.AddAnimation(0, "run", true, 0)
					//.SetListener(
					//	[](spine::AnimationState* state, spine::EventType type, spine::TrackEntry* entry, spine::Event* e) {
					//		spine::String animationName;
					//		if (entry && entry->getAnimation()) {
					//			animationName = entry->getAnimation()->getName();
					//		}
					//		switch (type) {
					//		case spine::EventType_Start:
					//			printf("%d start: %s\n", entry->getTrackIndex(), animationName.buffer());
					//			break;
					//		case spine::EventType_Interrupt:
					//			printf("%d interrupt: %s\n", entry->getTrackIndex(), animationName.buffer());
					//			break;
					//		case spine::EventType_End:
					//			printf("%d end: %s\n", entry->getTrackIndex(), animationName.buffer());
					//			break;
					//		case spine::EventType_Complete:
					//			printf("%d complete: %s\n", entry->getTrackIndex(), animationName.buffer());
					//			break;
					//		case spine::EventType_Dispose:
					//			printf("%d dispose: %s\n", entry->getTrackIndex(), animationName.buffer());
					//			break;
					//		case spine::EventType_Event:
					//			printf("%d event: %s, %s: %d, %f, %s %f %f\n", entry->getTrackIndex(), animationName.buffer(), e->getData().getName().buffer()
					//				, e->getIntValue(), e->getFloatValue(), e->getStringValue().buffer(), e->getVolume(), e->getBalance());
					//			break;
					//		}
					//		fflush(stdout);
					//	})
					;
				while (true) {
					co_yield 0;
					sp.Update(xx::engine.delta).Draw();
				}
				} (this, i));
		}

	}

	int Scene::Update() {
		xx::engine.PushBlendFunc();
		tasks();
		xx::engine.PopBlendFunc();
		return 0;
	}
}
