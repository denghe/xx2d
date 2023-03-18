#pragma once
#include "pch.h"
#include "xx2d_fps_viewer.h"

// data store file json structs
namespace MovePathStore {
	struct Point {
		int x{}, y{}, tension{ 20 }, numSegments{ 100 };
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
	int idx{};
};

struct GameLooper : xx::GameLooperBase {
	xx::BMFont fnt;
	xx::FpsViewer fpsViewer;

	std::optional<std::string> msg;
	bool exporting{};
	std::string exportFileName;

	::MovePathStore::Data data;
	std::string fileName, newLineName, changeLineName, selectedLineName;
	int selectedPointIdex{ -1 };
	xx::LineStrip lsPoint;
	xx::MovePath mp;
	std::vector<xx::CurvePoint> cps;
	double zoom{ 0.3 }, keyboardGCD{ 0.1 }, keyboardGCDNowSecs{};

	// affine trans
	xx::XY afPos{}, afScale{ 1, 1 };
	float afAngle{};

	DragableCircleMouseEventListener meListener;
	DragableCircle dc;

	inline static const float leftPanelWidth{ 480 }, margin{ 10 }, leftCmdPanelHeight1{ 80 }, leftCmdPanelHeight2{ 120 }, pointRadius{10.f};
	inline static const xx::XY errPanelSize{ 1200, 200 }, offset{ (leftPanelWidth + margin) / 2, 0 };
	inline static const xx::XY exportPanelSize{ 1200, 200 };

	inline static const ImVec4 normalColor{ 0, 0, 0, 1.0f };
	inline static const ImVec4 pressColor{ 0.5f, 0, 0, 1.0f };
	inline static const ImVec4 releaseColor{ 0, 0.5f, 0, 1.0f };

	void Init() override;
	int Update() override;

	void ImGuiUpdate();
	void ImGuiDrawWindow_Error();

	void ImGuiDrawWindow_LeftTop0();
	void ImGuiDrawWindow_LeftTop();

	void ImGuiDrawWindow_LeftBottom0();
	void ImGuiDrawWindow_LeftBottom();

	void ImGuiDrawWindow_Export();

	void LoadData();
	void SaveData();
	void ExportData();


	int UpdateLogic();

	void SelectLine(std::string_view const& name);
	int GetLineIndexByName(std::string_view const& name);
	int GetSelectedLineIndex();
	MovePathStore::Line* GetLineByName(std::string_view const& name);
	MovePathStore::Line* GetSelectedLine();
	bool CreateNewLine();
	void MakeNewLineName();
	int KeyboardGCDCheck();
	int SetKeyboardGCD();
};
