#include "pch.h"
#include "main.h"
#include "scene_points.h"

int main() {
	return GameLooper{}.Run("xx2d's movepath editor");
}

#include "ajson.hpp"
AJSON(::MovePathStore::Point, x, y, tension, numSegments);
AJSON(::MovePathStore::Line, name, isLoop, points);
AJSON(::MovePathStore::Group, name, lineNames);
AJSON(::MovePathStore::Data, designWidth, designHeight, safeLength, lines, groups);

void GameLooper::Init() {
	fnt = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	fpsViewer.Init(fnt);

	// load store data
	auto [d, p] = xx::engine.ReadAllBytes("res/movepath.json"sv);
	fileName = std::move(p);
	ajson::load_from_buff(data, (char*)d.buf, d.len);
	// make fixed memory for line edit & store
	data.lines.reserve(100000);
	data.groups.reserve(100000);

	scene.Emplace()->Init(this);

	xx::engine.imguiInit = [] { 
		auto&& io = ImGui::GetIO();
		io.Fonts->ClearFonts();

		ImFontConfig cfg;
		cfg.SizePixels = 26.f;
		auto&& imfnt = io.Fonts->AddFontDefault(&cfg);

		// auto&& imfnt = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 24, {}, io.Fonts->GetGlyphRangesChineseFull());

		io.Fonts->Build();
		io.FontDefault = imfnt;
	};

	xx::engine.imguiDeinit = [] {
		auto&& io = ImGui::GetIO();
		io.Fonts->ClearFonts();
	};

	xx::engine.imguiUpdate = [this] { ImGuiUpdate(); };
}

int GameLooper::Update() {
	if (int r = scene->Update()) return r;
	fpsViewer.Update();
	return 0;
}


void GameLooper::ImGuiUpdate() {

	ImGui::StyleColorsDark();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	auto sgStyleVar = xx::MakeScopeGuard([] { ImGui::PopStyleVar(1); });

	ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, releaseColor);
	auto sgStyleColor = xx::MakeScopeGuard([] { ImGui::PopStyleColor(2); });

	if (err.has_value()) {
		ImGuiDrawWindow_Error();
		return;
	}

	ImGuiDrawWindow_Left();
	ImGuiDrawWindow_Top();
	// ...
}

void GameLooper::ImGuiDrawWindow_Error() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
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
}

void GameLooper::ImGuiDrawWindow_Left() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	p.y += margin;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(400, xx::engine.h - 80));
	ImGui::Begin("lines", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	for (auto& p : data.lines) {
		ImGui::PushStyleColor(ImGuiCol_Button, &p == scene->line ? pressColor : normalColor);
		auto sg = xx::MakeScopeGuard([] { ImGui::PopStyleColor(1); });
		if (ImGui::Button(p.name.c_str())) {
			scene->SetLine(&p);
		}
	}

	ImGui::End();
}

void GameLooper::ImGuiDrawWindow_Top() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin + leftPanelWidth + margin;
	p.y += margin;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(xx::engine.w - p.x - margin, topPanelHeight));
	ImGui::Begin("tips1", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar);

	ImGui::Text("Z/X: Zooom in/out;  W/S: cursor up/down; C/V: copy/paste; F: clear");

	ImGui::End();
}
