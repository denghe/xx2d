#include "main.h"
#include "s17_imgui.h"

namespace ImGuiTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "ImGuiTest::Scene::Init" << std::endl;

		xx::engine.imguiUpdate = [this] {
			if (showDemoWindow) {
#ifdef XX2D_ENABLE_IMGUI
				ImGui::ShowDemoWindow(&showDemoWindow);
#endif
			}
		};
	}

	int Scene::Update() {
		return 0;
	}

	Scene::~Scene() {
		xx::engine.imguiUpdate = {};
	}
}
