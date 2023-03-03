#pragma once
#include "xx2d_pch.h"
#include "imgui.h"

namespace xx {
	void ImGuiInit(void* wnd);
	void ImGuiUpdate();
	void ImGuiRender();
	void ImGuiDraw();
	void ImGuiDestroy();
}
