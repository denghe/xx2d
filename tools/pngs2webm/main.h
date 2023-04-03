#pragma once
#include "pch.h"
#include "xx2d_fps_viewer.h"
#include "imgui.h"

struct GameLooper : xx::GameLooperBase {
	xx::BMFont fnt;
	xx::FpsViewer fpsViewer;

	std::optional<std::string> msg;
	std::string zoomString;
	std::string resPath;
	std::string fileNamePrefix;
	std::vector<std::string> files;
	std::string ratesString;
	int fps = 30;
	std::string fpsString;
	std::set<int> rates;
	std::string selectedFile;
	std::string selectedFileInfo;
	//xx::Webm mv;
	double zoom{ 0.3 }, keyboardGCD{ 0.2 }, keyboardGCDNowSecs{};
	xx::XY offset{};

	inline static const float leftPanelWidth{ 480 }, margin{ 10 }, rightTopPanelHeight{ 140 };
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

	//void DrawSelectedFile();
	//void ReloadFiles();
	//void ConvertFiles();
	//bool Convertable();
	//bool IsPic(std::string_view sv);
	//std::vector<std::string_view> GetPicFiles();
	//std::string_view GetSameName(std::vector<std::string_view> const& fs);
	//void FillRates();
	//void FillFps();
	//void FillDefaultResPath();

	void MoveUp();
	void MoveDown();
	void MoveTop();
	void MoveBottom();
	void ZoomOut();
	void ZoomIn();
};
