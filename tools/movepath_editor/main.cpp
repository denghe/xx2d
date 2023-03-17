#include "pch.h"
#include "main.h"
#include "imgui_stdlib.h"


int main() {
	return GameLooper{}.Run("xx2d'ss movepath editor");
}

#include "ajson.hpp"
AJSON(::MovePathStore::Point, x, y, tension, numSegments);
AJSON(::MovePathStore::Line, name, isLoop, points);
AJSON(::MovePathStore::Data, designWidth, designHeight, safeLength, lines);

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

	// 
	lsPoint.FillCirclePoints({}, pointRadius, {}, 16);

	meListener.Init(xx::Mbtns::Left);

	LoadData();
}

void GameLooper::LoadData() {
	// load store data
	auto [d, p] = xx::engine.ReadAllBytes("res/movepath.json"sv);
	fileName = std::move(p);
	data = {};
	ajson::load_from_buff(data, (char*)d.buf, d.len);
	// make fixed memory for easy access
	data.lines.reserve(100000);
}

void GameLooper::SaveData() {
	ajson::save_to_file(data, fileName.c_str());
}

void GameLooper::ExportData() {
	// todo
	//xx::WriteAllBytes()
	exporting = false;
}



int GameLooper::Update() {
	if (!err.has_value() && !exporting) {
		if (int r = UpdateLogic()) return r;
	}
	fpsViewer.Update();
	return 0;
}

void GameLooper::ImGuiUpdate() {

	if (err.has_value()) {
		ImGuiDrawWindow_Error();
		return;
	}

	if (exporting) {
		ImGuiDrawWindow_Export();
		return;
	}

	ImGuiDrawWindow_LeftTop0();
	ImGuiDrawWindow_LeftTop();
	ImGuiDrawWindow_LeftBottom0();
	ImGuiDrawWindow_LeftBottom();
	// ...
}

void GameLooper::ImGuiDrawWindow_Error() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += (xx::engine.w - errPanelSize.x) / 2;
	p.y += (xx::engine.h - errPanelSize.y) / 2;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(errPanelSize.x, errPanelSize.y));
	ImGui::Begin("error", nullptr, ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);
	ImGui::Text(err->c_str());
	if (ImGui::Button("ok")) {
		err.reset();
	}
	ImGui::End();
}

void GameLooper::ImGuiDrawWindow_Export() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += (xx::engine.w - exportPanelSize.x) / 2;
	p.y += (xx::engine.h - exportPanelSize.y) / 2;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(exportPanelSize.x, exportPanelSize.y));
	ImGui::Begin("export", nullptr, ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	ImGui::Text("please input the output file name:");
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("##exportFileName", &exportFileName);

	ImGui::Dummy({ 0.0f, 26.0f });
	ImGui::Separator();
	ImGui::Dummy({ 0.0f, 5.0f });

	ImGui::Dummy({ 0.0f, 0.0f });
	ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::GetStyle().ItemSpacing.x + 120 + 10 + 120));
	if (ImGui::Button("cancel", { 120, 35 })) {
		exporting = false;
	}
	ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::GetStyle().ItemSpacing.x + 120));
	if (ImGui::Button("export", { 120, 35 })) {
		ExportData();
	}

	ImGui::End();
}


void GameLooper::ImGuiDrawWindow_LeftTop0() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	p.y += margin;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, leftCmdPanelHeight1));

	ImGui::Begin("linescmds", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);
	
	auto sgBegin = xx::MakeSimpleScopeGuard([] {
		ImGui::End(); 
	});

	if (ImGui::Button("reload")) {
		std::string bak;
		if (line) {
			bak = line->name;
		}
		LoadData();
		if (line) {
			for (auto& l : data.lines) {
				if (l.name == bak) {
					line = &l;
					break;
				}
			}
		}
		copyLine = {};
	}
	ImGui::SameLine({}, 40);
	if (ImGui::Button("save")) {
		SaveData();
	}
	ImGui::SameLine({}, 10);
	if (ImGui::Button("export")) {
		exporting = true;
	}
	if (line) {
		ImGui::SameLine({}, 13);
		ImGui::PushStyleColor(ImGuiCol_Button, copyLine ? pressColor : normalColor);
		if (ImGui::Button( "copy")) {
			copyLine = line;
		}
		ImGui::PopStyleColor(1);
	}
	if (copyLine && line && line != copyLine && line->points.empty()) {
		ImGui::SameLine({}, 10);
		ImGui::PushStyleColor(ImGuiCol_Button, pressColor);
		if (ImGui::Button("paste")) {
			line->points.insert(line->points.begin(), copyLine->points.begin(), copyLine->points.end());
		}
		ImGui::PopStyleColor(1);
	}

	ImGui::Text("%s", "name:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(285);
	ImGui::InputText("##newLineName", &newLineName);
	ImGui::SameLine();
	if (ImGui::Button("create")) {
		if (newLineName.empty()) {
			err = "line name can't be null";
			return;
		}
		bool found = false;
		for (auto& l : data.lines) {
			if (l.name == newLineName) {
				err = "line name already exists";
				return;
			}
		}
		data.lines.emplace_back().name = newLineName;
	}

}

void GameLooper::ImGuiDrawWindow_LeftTop() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	p.y += margin + leftCmdPanelHeight1;
	auto h = (xx::engine.h - margin * 3 - (leftCmdPanelHeight1 + leftCmdPanelHeight2)) / 2;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, h));
	ImGui::Begin("lines", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	constexpr int numCols = 3;
	if (ImGui::BeginTable("linesstable", numCols, {}, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 7))) {
		ImGui::TableSetupColumn("select", ImGuiTableColumnFlags_WidthFixed, 60);
		ImGui::TableSetupColumn("line name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("delete", ImGuiTableColumnFlags_WidthFixed, 32);
		ImGui::TableHeadersRow();

		int rowId{}, removeRowId{ -1 };
		for (auto& p : data.lines) {

			ImGui::TableNextRow();

			int n = 0;

			ImGui::TableSetColumnIndex(n);
			ImGui::PushID(rowId * numCols + n);
			ImGui::PushStyleColor(ImGuiCol_Button, &p == line ? pressColor : normalColor);
			auto sg = xx::MakeScopeGuard([] { ImGui::PopStyleColor(1); });
			if (ImGui::Button("==>")) {
				SetLine(&p);
			}
			ImGui::PopID();

			++n;
			ImGui::TableSetColumnIndex(n);
			ImGui::PushID(rowId * numCols + n);
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::Text(p.name.c_str());
			ImGui::PopID();

			++n;
			ImGui::TableSetColumnIndex(n);
			ImGui::PushID(rowId * numCols + n);
			if (ImGui::Button("X")) {
				removeRowId = rowId;
			}
			ImGui::PopID();

			++rowId;
		}
		ImGui::EndTable();

		if (removeRowId >= 0) {
			auto iter = data.lines.begin() + removeRowId;
			if (line && &(*iter) == line) {
				line = {};
			}
			if (copyLine && &(*iter) == copyLine) {
				copyLine = {};
			}
			data.lines.erase(iter);
		}
	}

	ImGui::End();
}

void GameLooper::ImGuiDrawWindow_LeftBottom0() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	auto h = (xx::engine.h - margin * 3 - (leftCmdPanelHeight1 + leftCmdPanelHeight2)) / 2;
	p.y += margin * 2 + leftCmdPanelHeight1 + h;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, leftCmdPanelHeight2));
	ImGui::Begin("pointscmds", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	if (line) {

		ImGui::SetNextItemWidth(296);
		ImGui::InputText("##newLineName2", &changeLineName);
		ImGui::SameLine();
		if (ImGui::Button("change name")) {
			if (changeLineName.empty()) {
				err = "line name can't be null";
			}
			bool found = false;
			for (auto& l : data.lines) {
				if (&l == line) continue;
				if (l.name == changeLineName) {
					err = "line name already exists";
					found = true;
					break;
				}
			}
			if (!found) {
				line->name = changeLineName;
			}
		}


		ImGui::Checkbox("loop", &line->isLoop);
		ImGui::SameLine({}, 28);
		if (ImGui::Button("new point")) {
			line->points.emplace_back();
			selectedPoint = {};
		}
		ImGui::SameLine({}, 28);
		if (ImGui::Button("flipX")) {
			for (auto& p : line->points) {
				p.x = -p.x;
			}
		}
		ImGui::SameLine({}, 28);
		if (ImGui::Button("flipY")) {
			for (auto& p : line->points) {
				p.y = -p.y;
			}
		}

		// todo: rotate? move all?
		ImGui::Text("p");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afPosx", &afPos.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afPosy", &afPos.y);
		ImGui::SameLine();
		ImGui::Text("s");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afScalex", &afScale.x);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afScaley", &afScale.y);
		ImGui::SameLine();
		ImGui::Text("r");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afAngle", &afAngle);
		ImGui::SameLine();
		if (ImGui::Button("go")) {
			auto at = xx::AffineTransform::MakePosScaleRadians(afPos, afScale, afAngle * M_PI / 360.f);
			for (auto& p : line->points) {
				auto pos = at.Apply(xx::XY{ (float)p.x, (float)p.y });
				p.x = pos.x;
				p.y = pos.y;
			}
		}
	}

	ImGui::End();
}

void GameLooper::ImGuiDrawWindow_LeftBottom() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	auto h = (xx::engine.h - margin * 3 - (leftCmdPanelHeight1 + leftCmdPanelHeight2)) / 2;
	p.y += margin * 2 + leftCmdPanelHeight1 + leftCmdPanelHeight2 + h;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, h));
	ImGui::Begin("points", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	if (line) {

		constexpr int numCols = 7;
		if (ImGui::BeginTable("pointstable", numCols, {}, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 7))) {

			ImGui::TableSetupColumn("select", ImGuiTableColumnFlags_WidthFixed, 32);
			ImGui::TableSetupColumn("insert", ImGuiTableColumnFlags_WidthFixed, 32);
			ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed, 80);
			ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthFixed, 80);
			ImGui::TableSetupColumn("tension", ImGuiTableColumnFlags_WidthFixed, 70);
			ImGui::TableSetupColumn("numSegments", ImGuiTableColumnFlags_WidthFixed, 70);
			ImGui::TableSetupColumn("delete", ImGuiTableColumnFlags_WidthFixed, 32);
			ImGui::TableHeadersRow();

			int rowId{}, removeRowId{ -1 };
			for (auto& p : line->points) {

				ImGui::TableNextRow();
				ImGui::PushStyleColor(ImGuiCol_Button, selectedPoint == &p ? pressColor : normalColor);

				int n = 0;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				if (ImGui::Button(">")) {
					selectedPoint = &p;
				}
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				if (ImGui::Button("+")) {
					line->points.insert(line->points.begin() + rowId, MovePathStore::Point{});
					selectedPoint = {};
				}
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputInt("##", &p.x, 0, 0);
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputInt("##", &p.y, 0, 0);
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputInt("##", &p.tension, 0.0f, 0.0f);
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputInt("##", &p.numSegments, 0, 0);
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				if (ImGui::Button("X")) {
					removeRowId = rowId;
					selectedPoint = {};
				}
				ImGui::PopID();

				ImGui::PopStyleColor(1);
				++rowId;
			}
			ImGui::EndTable();

			if (removeRowId >= 0) {
				line->points.erase(line->points.begin() + removeRowId);
			}
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

	}

	meListener.Update();
	dc.looper = this;
	int i = 0;
	while (meListener.eventId && i != line->points.size()) {
		dc.point = &line->points[i];
		meListener.Dispatch(&dc);
		++i;
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
		cps.emplace_back(pos, (float)p.tension / 100.f, (int32_t)p.numSegments);
		lsPoint.SetPosition(pos * zoom + offset).SetColor(&p == selectedPoint ? xx::RGBA8{ 255,0,0,255 } : xx::RGBA8{ 0,255,255,255 }).Draw();
	}
	if (line->points.size() < 2) return 0;
	mp.Clear();
	mp.FillCurve(line->isLoop, cps);

	// draw body segments
	if (mp.totalDistance > 1.f) {
		xx::MovePathCache mpc;
		mpc.Init(mp);

		auto& ps = ls.Clear().SetPoints();
		for (auto& p : mpc.points) {
			ps.emplace_back(p.pos);
		}
		ls.SetColor({ 255,255,255,255 }).SetPosition(offset).SetScale(zoom).Draw();
	}

	xx::SimpleLabel lbl;
	lbl.SetScale({ 0.85, 1 }).SetAnchor({ 0,0 }).SetColor({ 127,127,0,255 })
		.SetPosition(xx::engine.ninePoints[1].MakeAdd(leftPanelWidth + margin * 2, 32 + margin))
		.SetText(fnt, xx::ToString("zoom = ", zoom, ", points.size() = ", line->points.size()))
		.Draw()
		.SetAnchor({ 0,1 })
		.SetPosition(xx::engine.ninePoints[7].MakeAdd(leftPanelWidth + margin * 2, -margin))
		.SetText(fnt, "Z/X: Zooom in/out"sv)
		.Draw();
	return 0;
}

void GameLooper::SetLine(MovePathStore::Line* const& line_) {
	if (line == line_) return;
	line = line_;
	if (!line) return;
	changeLineName = line->name;
	selectedPoint = {};
}


bool DragableCircle::HandleMouseDown(DragableCircleMouseEventListener& L) {
	auto dx = looper->offset.x + point->x * looper->zoom - L.downPos.x;
	auto dy = looper->offset.y + point->y * looper->zoom - L.downPos.y;
	if (dx * dx + dy * dy < GameLooper::pointRadius * GameLooper::pointRadius) {
		pos = xx::XY{ (float)point->x, (float)point->y };
		looper->selectedPoint = point;
		return true;
	}
	return false;
}
int DragableCircle::HandleMouseMove(DragableCircleMouseEventListener& L) {
	pos += (xx::engine.mousePosition - L.lastPos) / looper->zoom;
	point->x = pos.x;
	point->y = pos.y;
	return 0;
}
void DragableCircle::HandleMouseUp(DragableCircleMouseEventListener& L) {
	point = {};
}
