#pragma once
#include "pch.h"
#include "xx2d_fps_viewer.h"

// data store file json structs
namespace MovePathStore {
	struct Point {
		float x{}, y{}, tension{};
		uint32_t numSegments{};
	};
	struct Line {
		std::string name;
		bool isLoop{};
		std::vector<Point> points;
	};
	struct Group {
		std::string name;
		std::vector<std::string> lineNames;	// old name: lineIndexs
	};
	struct Data {
		uint32_t designWidth{}, designHeight{}, safeLength{};
		std::vector<Line> lines;
		std::vector<Group> groups;
	};
}

struct Scene_Points;
struct GameLooper : xx::GameLooperBase {
	xx::BMFont fnt;
	xx::FpsViewer fpsViewer;

	xx::Shared<Scene_Points> scene;

	std::optional<std::string> err;
	::MovePathStore::Data data;
	std::string fileName;

	inline static const float leftPanelWidth{ 400 }, margin{ 20 }, topPanelHeight{ 40 };

	inline static const ImVec4 normalColor{ 0, 0, 0, 1.0f };
	inline static const ImVec4 pressColor{ 0.5f, 0, 0, 1.0f };
	inline static const ImVec4 releaseColor{ 0, 0.5f, 0, 1.0f };

	void Init() override;
	int Update() override;

	void ImGuiUpdate();
	void ImGuiDrawWindow_Error();
	void ImGuiDrawWindow_Left();
	void ImGuiDrawWindow_Top();
};
