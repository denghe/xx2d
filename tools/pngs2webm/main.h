#pragma once
#include "pch.h"
#include "xx2d_fps_viewer.h"
#include "imgui.h"

struct GameLooper : xx::GameLooperBase {
	xx::BMFont fnt;
	xx::FpsViewer fpsViewer;

	std::optional<std::string> msg;

	std::string fileNamePrefix;
	double zoom{ 0.3 }, keyboardGCD{ 0.2 }, keyboardGCDNowSecs{};

	inline static const float leftPanelWidth{ 480 }, margin{ 10 }, rightTopPanelHeight{ 120 };
	inline static const xx::XY errPanelSize{ 1200, 200 };

	inline static const ImVec4 normalColor{ 0, 0, 0, 1.0f };
	inline static const ImVec4 pressColor{ 0.5f, 0, 0, 1.0f };
	inline static const ImVec4 releaseColor{ 0, 0.5f, 0, 1.0f };

	void Init() override;
	int Update() override;

	void ImGuiUpdate();
	void ImGuiDrawWindow_Error();

	void ImGuiDrawWindow_Left();
	void ImGuiDrawWindow_RightTop();
	void ImGuiDrawWindow_RightBottom();

	int KeyboardGCDCheck();
	int SetKeyboardGCD();
};
