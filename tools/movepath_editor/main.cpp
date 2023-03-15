#include "main.h"

int main() {
	return GameLooper{}.Run();
}

#include "ajson.hpp"
AJSON(::MovePathStore::Point, x, y, tension, numSegments);
AJSON(::MovePathStore::Line, name, isLoop, points);
AJSON(::MovePathStore::Group, name, lineNames);
AJSON(::MovePathStore::Data, designWidth, designHeight, safeLength, lines, groups);

void GameLooper::Init() {
	fnt = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	fpsViewer.Init(fnt);

	auto fn = xx::engine.GetFullPath("res/movepath.json"sv);
	ajson::load_from_file(data, fn.c_str());

	xx::engine.imguiInit = [this] { 
		auto&& io = ImGui::GetIO();
		imfnt = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 32, {}, io.Fonts->GetGlyphRangesChineseFull());
		io.Fonts->Build();
		io.FontDefault = imfnt;
	};

	xx::engine.imguiDeinit = [this] {
		auto&& io = ImGui::GetIO();
		io.Fonts->ClearFonts();
	};

	xx::engine.imguiUpdate = [this] { ImGuiUpdate(); };
}

int GameLooper::Update() {
	if (scene) {
		if (int r = scene->Update()) return r;
	}
	fpsViewer.Update();
	return 0;
}

void GameLooper::ImGuiUpdate() {

	ImGui::StyleColorsDark();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	auto sgStyleVar = xx::MakeScopeGuard([] { ImGui::PopStyleVar(1); });

	static const ImVec4 normalColor{ 0, 0, 0, 1.0f };
	static const ImVec4 pressColor{ 0.5f, 0, 0, 1.0f };
	static const ImVec4 releaseColor{ 0, 0.5f, 0, 1.0f };

	ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, releaseColor);
	auto sgStyleColor = xx::MakeScopeGuard([] { ImGui::PopStyleColor(2); });

	ImGuiDrawWindow_Error();

	// todo
}

	//scene = xx::Make<Scene_Menu>();
	//scene->Init(this);

void GameLooper::ImGuiDrawWindow_Error() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	if (err.has_value()) {
		p.x += 300;
		p.y += 450;
		ImGui::SetNextWindowPos(p);
		ImGui::SetNextWindowSize(ImVec2(xx::engine.w - 300 * 2, xx::engine.h - 450 * 2));
		ImGui::Begin("Error", nullptr, ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize);
		ImGui::Text(err->c_str());
		ImGui::End();
		return;
	}
}
