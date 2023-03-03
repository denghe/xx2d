﻿#include "xx2d_pch.h"
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace xx {

	void ImGuiInit(void* wnd) {
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
	}

	void ImGuiUpdate() {
		if (!xx::engine.imguiLogic) return;

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//if (show_demo_window)
		//	ImGui::ShowDemoWindow(&show_demo_window);
		xx::engine.imguiLogic();
		//// Rendering
		ImGui::Render();
	}

	void ImGuiDraw() {
		if (!xx::engine.imguiLogic) return;

		if (auto d = ImGui::GetDrawData()) {
			ImGui_ImplOpenGL3_RenderDrawData(d);
		}
	}

	void ImGuiDestroy() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}
