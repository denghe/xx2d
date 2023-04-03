#include "pch.h"
#include "main.h"
#include "imgui_stdlib.h"

/*
main ui layout:

┌────────────────────┬────────────────────────────────────────────────────────────────┐
│                    │ [ ... file path prefix                                ] filter │
│                    │ [ 20, 50, 100, 200... rates                          ] convert │
│                    │ selected file info                                             │
│                    ├────────────────────────────────────────────────────────────────┤
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│    files list      │                                                                │
│                    │                           preview area                         │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │                                                                │
│                    │  [ rotate + - 0 ]                              [ zoom + - 0 ]  │
└────────────────────┴────────────────────────────────────────────────────────────────┘
*/

int main() {
	return GameLooper{}.Run("xx2d's pngs to webm / res viewer");
}

// todo

void GameLooper::Init() {

	xx::engine.imguiInit = [] { 
		auto&& io = ImGui::GetIO();
		io.Fonts->ClearFonts();
		ImFontConfig cfg;
		cfg.SizePixels = 26.f;	// make font size bigger than default
		auto&& imfnt = io.Fonts->AddFontDefault(&cfg);
		io.Fonts->Build();
		io.FontDefault = imfnt;
	};

	xx::engine.imguiDeinit = [] {
		auto&& io = ImGui::GetIO();
		io.Fonts->ClearFonts();
	};

	xx::engine.imguiUpdate = [this] { ImGuiUpdate(); };

	fnt = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	fpsViewer.Init(fnt, leftPanelWidth + margin * 2);
}

int GameLooper::Update() {
	// todo: right button content draw
	
	fpsViewer.Update();
	return 0;
}

void GameLooper::ImGuiUpdate() {

	if (msg.has_value()) {
		ImGuiDrawWindow_Error();
		return;
	}

	ImGuiDrawWindow_Left();
	//ImGuiDrawWindow_RightTop();
	//ImGuiDrawWindow_RightBottom();
	// ...
}

void GameLooper::ImGuiDrawWindow_Error() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += (xx::engine.w - errPanelSize.x) / 2;
	p.y += (xx::engine.h - errPanelSize.y) / 2;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(errPanelSize.x, errPanelSize.y));
	ImGui::Begin("dialog", nullptr, ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	ImGui::Text(msg->c_str());

	ImGui::Dummy({ 0.0f, 62.0f });
	ImGui::Separator();
	ImGui::Dummy({ 0.0f, 5.0f });

	ImGui::Dummy({ 0.0f, 0.0f });
	ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::GetStyle().ItemSpacing.x + 90 + 10));
	if (ImGui::Button("close", { 90, 35 })) {
		msg.reset();
	}
	ImGui::End();
}

void GameLooper::ImGuiDrawWindow_Left() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	p.y += margin;
	auto&& h = xx::engine.h - margin * 2;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, h));
	ImGui::Begin("lines", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	constexpr int numCols = 2;
	if (ImGui::BeginTable("filestable", numCols, {}, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 7))) {
		ImGui::TableSetupColumn("select", ImGuiTableColumnFlags_WidthFixed, 60);
		ImGui::TableSetupColumn("file name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		//int rowId{}, removeRowId{ -1 };
		//for (auto& p : data.lines) {

		//	ImGui::TableNextRow();

		//	int n = 0;

		//	ImGui::TableSetColumnIndex(n);
		//	ImGui::PushID(rowId * numCols + n);
		//	ImGui::PushStyleColor(ImGuiCol_Button, p.name == selectedLineName ? pressColor : normalColor);
		//	auto&& sg = xx::MakeScopeGuard([] { ImGui::PopStyleColor(1); });
		//	if (ImGui::Button("==>")) {
		//		SelectLine(p.name);
		//	}
		//	ImGui::PopID();

		//	++n;
		//	ImGui::TableSetColumnIndex(n);
		//	ImGui::PushID(rowId * numCols + n);
		//	ImGui::SetNextItemWidth(-FLT_MIN);
		//	ImGui::Text(p.name.c_str());
		//	ImGui::PopID();

		//	++rowId;
		//}
		ImGui::EndTable();
	}

	ImGui::End();
}

int GameLooper::KeyboardGCDCheck() {
	if (keyboardGCDNowSecs < xx::engine.nowSecs) return SetKeyboardGCD();
	return 0;
}
int GameLooper::SetKeyboardGCD() {
	keyboardGCDNowSecs = xx::engine.nowSecs + keyboardGCD;
	return 1;
}
