#include "pch.h"
#include "main.h"
#include "imgui_stdlib.h"


int main() {
	return GameLooper{}.Run("xx2d'ss movepath editor");
}

#include "ajson.hpp"
AJSON(::MovePathStore::Point, x, y, tension, numSegments);
AJSON(::MovePathStore::Line, name, isLoop, points);
AJSON(::MovePathStore::Group, name, lineNames);
AJSON(::MovePathStore::Data, designWidth, designHeight, safeLength, lines, groups);

void GameLooper::Init() {

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


	// load fnt
	fnt = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	fpsViewer.Init(fnt, leftPanelWidth + margin * 2);

	// load store data
	auto [d, p] = xx::engine.ReadAllBytes("res/movepath.json"sv);
	fileName = std::move(p);
	ajson::load_from_buff(data, (char*)d.buf, d.len);
	// make fixed memory for line edit & store
	data.lines.reserve(100000);
	data.groups.reserve(100000);

	// 
	lsPoint.FillCirclePoints({}, 10, {}, 16);
}

int GameLooper::Update() {
	if (int r = UpdateLogic()) return r;
	fpsViewer.Update();
	return 0;
}


void GameLooper::ImGuiUpdate() {

	ImGui::PushStyleColor(ImGuiCol_Button, normalColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, releaseColor);
	auto sgStyleColor = xx::MakeScopeGuard([] { ImGui::PopStyleColor(3); });

	if (err.has_value()) {
		ImGuiDrawWindow_Error();
		return;
	}

	ImGuiDrawWindow_LeftTop();
	ImGuiDrawWindow_LeftBottom();
	// ...
}

void GameLooper::ImGuiDrawWindow_Error() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += (xx::engine.w - errPanelSize.x) / 2;
	p.y += (xx::engine.h - errPanelSize.y) / 2;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(errPanelSize.x, errPanelSize.y));
	ImGui::Begin("Error", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);
	ImGui::Text(err->c_str());
	ImGui::End();
}

void GameLooper::ImGuiDrawWindow_LeftTop() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	p.y += margin;
	auto h = (xx::engine.h - margin * 3) / 2;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, h));
	ImGui::Begin("lines", nullptr, ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	for (auto& p : data.lines) {
		ImGui::PushStyleColor(ImGuiCol_Button, &p == line ? pressColor : normalColor);
		auto sg = xx::MakeScopeGuard([] { ImGui::PopStyleColor(1); });
		if (ImGui::Button(p.name.c_str())) {
			SetLine(&p);
		}
	}

	ImGui::End();
}

void GameLooper::ImGuiDrawWindow_LeftBottom() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	auto h = (xx::engine.h - margin * 3) / 2;
	p.y += margin * 2 + h;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, h));
	ImGui::Begin("points", nullptr, ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	if (line) {
		if (ImGui::BeginTable("table2", 4, {}, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 7))) {

			auto cb = [](ImGuiInputTextCallbackData* data)->int {
				std::string s(data->Buf, data->BufTextLen);
				auto& pair = *(std::pair<MovePathStore::Point*, int>*)data->UserData;
				switch (pair.second) {
				case 0:
					xx::Convert(s.c_str(), pair.first->x);
					break;
				case 1:
					xx::Convert(s.c_str(), pair.first->y);
					break;
				case 2:
					xx::Convert(s.c_str(), pair.first->tension);
					break;
				case 3:
					xx::Convert(s.c_str(), pair.first->numSegments);
					break;
				default:
					throw std::logic_error("unknown type id");
				}
				return 1;
			};
			auto flag = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CallbackCompletion;

			int rowId = 0;
			for (auto& p : line->points) {
				p.sx = xx::ToString(p.x);
				p.sy = xx::ToString(p.y);
				p.st = xx::ToString(p.tension);
				p.sn = xx::ToString(p.numSegments);
				p.px = { &p, 0 };
				p.py = { &p, 1 };
				p.pt = { &p, 2 };
				p.pn = { &p, 3 };

				ImGui::TableNextColumn();
				ImGui::PushID(rowId * 4 + 0);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputText("##", &p.sx, flag, cb, &p.px);
				ImGui::PopID();

				ImGui::TableNextColumn();
				ImGui::PushID(rowId * 4 + 1);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputText("##", &p.sy, flag, cb, &p.py);
				ImGui::PopID();

				ImGui::TableNextColumn();
				ImGui::PushID(rowId * 4 + 2);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputText("##", &p.st, flag, cb, &p.pt);
				ImGui::PopID();

				ImGui::TableNextColumn();
				ImGui::PushID(rowId * 4 + 3);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputText("##", &p.sn, flag, cb, &p.pn);
				ImGui::PopID();

				++rowId;
			}
			ImGui::EndTable();
		}
	}

	ImGui::End();
}





int GameLooper::UpdateLogic() {
	if (!line) return 0;

	timePool += xx::engine.delta;
	while (timePool >= 1.f / 60) {
		timePool -= 1.f / 60;

		if (xx::engine.Pressed(xx::KbdKeys::Z)) {
			zoom += 0.02;
			if (zoom >= 2) {
				zoom = 2;
			}
		}
		if (xx::engine.Pressed(xx::KbdKeys::X)) {
			zoom -= 0.02;
			if (zoom <= 0.3) {
				zoom = 0.3;
			}
		}

		// todo: edit logic
	}

	// draw begin
	xx::XY offset{ (leftPanelWidth + margin) / 2, 0 };
	xx::LineStrip ls;
	ls.SetScale(zoom).SetPosition(offset);

	// draw bg
	float dw = data.designWidth, dh = data.designHeight, sl = data.safeLength;
	float dw2 = dw / 2, dh2 = dh / 2;
	float r1 = std::sqrt(dw2 * dw2 + dh2 * dh2);
	float r2 = r1 + sl;
	float w = dw + sl, h = dh + sl;
	float w2 = w / 2, h2 = h / 2;
	ls.SetColor({ 255,127,127,255 }).SetPoints({ {-r2, 0}, {r2, 0} }).Draw();
	ls.SetPoints({ {0, -r2}, {0, r2} }).Draw();
	ls.SetColor({ 127,127,255,255 }).FillBoxPoints({}, { dw, dh }).Draw();
	ls.SetColor({ 127,255,127,255 }).FillCirclePoints({}, r1).Draw();
	ls.SetColor({ 255,255,127,255 }).FillCirclePoints({}, r2).Draw();

	// draw points
	cps.clear();
	for (auto& p : line->points) {
		xx::XY pos{ (float)p.x, (float)p.y };
		cps.emplace_back(pos, (float)p.tension, (int32_t)p.numSegments);
		lsPoint.SetPosition(pos * zoom + offset).SetColor({ 255,255,0,255 }).Draw();
	}
	mp.Clear();
	mp.FillCurve(line->isLoop, cps);

	// draw body segments
	if (mp.totalDistance < 0.001f) return 0;

	xx::MovePathCache mpc;
	mpc.Init(mp);

	auto& ps = ls.Clear().SetPoints();
	for (auto& p : mpc.points) {
		ps.emplace_back(p.pos);
	}
	ls.SetColor({ 255,255,255,255 }).SetPosition(offset).SetScale(zoom).Draw();

	xx::SimpleLabel lbl;
	lbl.SetScale({ 0.85, 1 }).SetAnchor({ 0,0 }).SetColor({ 127,127,0,255 })
		.SetPosition(xx::engine.ninePoints[1].MakeAdd(leftPanelWidth + margin * 2, 32 + margin))
		.SetText(fnt, xx::ToString("zoom = ", zoom, ", points.size() = ", line->points.size()))
		.Draw()
		.SetAnchor({ 0,1 })
		.SetPosition(xx::engine.ninePoints[7].MakeAdd(leftPanelWidth + margin * 2, -margin))
		.SetText(fnt, "Z/X: Zooom in/out;  W/S: cursor up/down; C/V: copy/paste; F: clear"sv)
		.Draw();
	return 0;
}

void GameLooper::SetLine(MovePathStore::Line* const& line_) {
	if (line == line_) return;
	line = line_;
	if (!line) return;
	// todo: clear points edit state ?
}
