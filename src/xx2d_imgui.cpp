#include "xx2d.h"
#ifdef XX2D_ENABLE_IMGUI
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif

namespace xx {

	void ImGuiInit(void* wnd) {
#ifdef XX2D_ENABLE_IMGUI
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)wnd, true);
		ImGui_ImplOpenGL3_Init("#version 300 es");
#endif
	}

	void ImGuiUpdate() {
#ifdef XX2D_ENABLE_IMGUI
		if (!xx::engine.imguiLogic) return;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		xx::engine.imguiLogic();
		ImGui::Render();
#endif
	}

	void ImGuiDraw() {
#ifdef XX2D_ENABLE_IMGUI
		if (!xx::engine.imguiLogic) return;

		if (auto d = ImGui::GetDrawData()) {
			ImGui_ImplOpenGL3_RenderDrawData(d);
		}
#endif
	}

	void ImGuiDestroy() {
#ifdef XX2D_ENABLE_IMGUI
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
#endif
	}
}
