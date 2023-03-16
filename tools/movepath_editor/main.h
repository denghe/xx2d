#pragma once
#include "pch.h"
#include "xx2d_fps_viewer.h"

// data store file json structs
namespace MovePathStore {
	struct Point {
		int x{}, y{};
		float tension{ 0.3f };
		int numSegments{ 100 };
	};
	struct Line {
		std::string name;
		bool isLoop{};
		std::vector<Point> points;
	};
	struct Data {
		uint32_t designWidth{}, designHeight{}, safeLength{};
		std::vector<Line> lines;
	};
}

struct DragableCircle;
using DragableCircleMouseEventListener = xx::MouseEventListener<DragableCircle*>;

struct GameLooper;
struct DragableCircle {
	bool HandleMouseDown(DragableCircleMouseEventListener& L);
	int HandleMouseMove(DragableCircleMouseEventListener& L);
	void HandleMouseUp(DragableCircleMouseEventListener& L);
	GameLooper* looper{};
	MovePathStore::Point* point{};
	xx::XY pos{};
};

struct GameLooper : xx::GameLooperBase {
	xx::BMFont fnt;
	xx::FpsViewer fpsViewer;

	std::optional<std::string> err;
	::MovePathStore::Data data;
	std::string fileName, newLineName, changeLineName;

	inline static const float leftPanelWidth{ 400 }, margin{ 10 }, leftCmdPanelHeight{ 80 }, pointRadius{10.f};
	inline static const xx::XY errPanelSize{ 400, 300 }, offset{ (leftPanelWidth + margin) / 2, 0 };

	inline static const ImVec4 normalColor{ 0, 0, 0, 1.0f };
	inline static const ImVec4 pressColor{ 0.5f, 0, 0, 1.0f };
	inline static const ImVec4 releaseColor{ 0, 0.5f, 0, 1.0f };

	void Init() override;
	int Update() override;

	void ImGuiUpdate();
	void ImGuiDrawWindow_Error();
	void ImGuiDrawWindow_LeftCmd();
	void ImGuiDrawWindow_LeftTop();
	void ImGuiDrawWindow_LeftBottom();

	void LoadData();
	void SaveData();

	xx::LineStrip lsPoint;
	xx::MovePath mp;
	std::vector<xx::CurvePoint> cps;
	MovePathStore::Line* line{};
	double zoom{ 0.3 }, timePool{};

	DragableCircleMouseEventListener meListener;
	DragableCircle dc;

	int UpdateLogic();
	void SetLine(MovePathStore::Line* const& line);
};
