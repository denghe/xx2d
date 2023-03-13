#pragma once
#include "xx2d.h"
#include "imgui.h"

namespace xx {
	void ImGuiInit(void* wnd);
	void ImGuiUpdate();
	void ImGuiRender();
	void ImGuiDraw();
	void ImGuiDestroy();
}
