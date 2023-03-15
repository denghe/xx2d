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

//
struct GameLooper;
struct SceneBase {
	GameLooper* looper{};
	virtual void Init(GameLooper*) = 0;
	virtual int Update() = 0;
	virtual ~SceneBase() {};
};

//
struct GameLooper : xx::GameLooperBase {
	xx::BMFont fnt;
	xx::FpsViewer fpsViewer;

	xx::Shared<SceneBase> scene;

	std::optional<std::string> err;
	::MovePathStore::Data data;
	std::string fileName;
	std::string currLineName;

	inline static const ImVec4 normalColor{ 0, 0, 0, 1.0f };
	inline static const ImVec4 pressColor{ 0.5f, 0, 0, 1.0f };
	inline static const ImVec4 releaseColor{ 0, 0.5f, 0, 1.0f };

	template<typename LT>
	void DelaySwitchTo() {
		xx::engine.DelayExecute([this] {
			scene = xx::Make<LT>();
			scene->Init(this);
		});
	}

	void Init() override;
	int Update() override;

	void ImGuiUpdate();
	void ImGuiDrawWindow_Error();
	void ImGuiDrawWindow_Left();
	void ImGuiDrawWindow_Top();
};
