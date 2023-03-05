#include "xx2d_pch.h"
#include "game_looper.h"
#include "s17_imgui.h"
#include "xx2d_imgui.h"

namespace ImGuiTest {

	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "ImGuiTest::Scene::Init" << std::endl;

		xx::engine.imguiLogic = [this] {
			if (showDemoWindow) {
#ifdef XX_ENABLE_IMGUI
				ImGui::ShowDemoWindow(&showDemoWindow);
#endif
			}
		};
	}

	int Scene::Update() {
		return 0;
	}

	Scene::~Scene() {
		xx::engine.imguiLogic = {};
	}
}
