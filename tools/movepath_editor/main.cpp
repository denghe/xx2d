#include "pch.h"
#include "main.h"
#include "imgui_stdlib.h"

// todo: line icon ( need runtime generate texture ? )

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

	fnt = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	fpsViewer.Init(fnt, leftPanelWidth + margin * 2);

	lsPoint.FillCirclePoints({}, pointRadius, {}, 16);

	meListener.Init(xx::Mbtns::Left);

	LoadData();
	exportFileName = xx::engine.GetFullPath("res", false) + "/movepath.bin";
	MakeNewLineName();
}

void GameLooper::LoadData() {
	auto&& [d, p] = xx::engine.ReadAllBytes("res/movepath.json"sv);
	fileName = std::move(p);
	data = {};
	ajson::load_from_buff(data, (char*)d.buf, d.len);
}

void GameLooper::SaveData() {
	ajson::save_to_file(data, fileName.c_str());
}

// for export MovePathStore
namespace xx {
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<::MovePathStore::Data, std::decay_t<T>>>> {
		template<bool needReserve = true> static inline void Write(Data& d, T const& in) {
			d.Write(in.lines);
		}
	};
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<::MovePathStore::Line, std::decay_t<T>>>> {
		template<bool needReserve = true> static inline void Write(Data& d, T const& in) {
			d.Write(in.name, in.isLoop, in.points);
		}
	};
	template<typename T>
	struct DataFuncs<T, std::enable_if_t<std::is_same_v<::MovePathStore::Point, std::decay_t<T>>>> {
		template<bool needReserve = true> static inline void Write(Data& d, T const& in) {
			d.Write(in.x, in.y, in.tension, in.numSegments);
		}
	};
}

void GameLooper::ExportData() {
	// format: lines size + Line{ name size + name content + 1 byte isloop + points size + Point{ x,y,t,n }... }...
	xx::Data d;
	d.Write(data);
	auto&& r = xx::WriteAllBytes(exportFileName, d);
	if (r) {
		msg = xx::ToString("WriteAllBytes to file: ", exportFileName, " error! r = ", r);
	} else {
		msg = xx::ToString("export success!");
		exporting = false;
	}
}



int GameLooper::Update() {
	if (!msg.has_value() && !exporting) {
		if (int r = UpdateLogic()) return r;
	}
	fpsViewer.Update();
	return 0;
}

void GameLooper::ImGuiUpdate() {

	if (msg.has_value()) {
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
	
	auto&& sgBegin = xx::MakeSimpleScopeGuard([] {
		ImGui::End(); 
	});

	if (ImGui::Button("reload")) {
		LoadData();
		if (GetSelectedLineIndex() == -1) {
			selectedLineName.clear();
			ClearEditState();
		}
	}
	ImGui::SameLine({}, 40);
	if (ImGui::Button("save")) {
		SaveData();
	}
	ImGui::SameLine({}, 38);
	if (ImGui::Button("export")) {
		exporting = true;
	}
	if (selectedLineName.size()) {
		ImGui::SameLine({}, 60);
		if (ImGui::Button( "clone")) {
			if (CreateNewLine()) {
				auto&& l = GetSelectedLine();
				auto&& t = data.lines.back();
				t.isLoop = l->isLoop;
				t.points = l->points;
				SelectLine(t.name);
			}
		}
	}

	ImGui::Text("%s", "name:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(286);
	ImGui::InputText("##newLineName", &newLineName);
	ImGui::SameLine();
	if (ImGui::Button("create")) {
		if (CreateNewLine()) {
			SelectLine(data.lines.back().name);
		}
	}

}

void GameLooper::ImGuiDrawWindow_LeftTop() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	p.y += margin + leftCmdPanelHeight1;
	auto&& h = (xx::engine.h - margin * 3 - (leftCmdPanelHeight1 + leftCmdPanelHeight2)) / 2;
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
			ImGui::PushStyleColor(ImGuiCol_Button, p.name == selectedLineName ? pressColor : normalColor);
			auto&& sg = xx::MakeScopeGuard([] { ImGui::PopStyleColor(1); });
			if (ImGui::Button("==>")) {
				SelectLine(p.name);
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
			auto&& iter = data.lines.begin() + removeRowId;
			if (iter->name == selectedLineName) {
				selectedLineName.clear();
				ClearEditState();
			}
			data.lines.erase(iter);
		}
	}

	ImGui::End();
}

void GameLooper::ImGuiDrawWindow_LeftBottom0() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin;
	auto&& h = (xx::engine.h - margin * 3 - (leftCmdPanelHeight1 + leftCmdPanelHeight2)) / 2;
	p.y += margin * 2 + leftCmdPanelHeight1 + h;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, leftCmdPanelHeight2));
	ImGui::Begin("pointscmds", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	if (selectedLineName.size()) {
		auto&& line = GetLineByName(selectedLineName);

		ImGui::SetNextItemWidth(296);
		ImGui::InputText("##newLineName2", &changeLineName);
		ImGui::SameLine();
		if (ImGui::Button("change name")) {
			if (changeLineName.empty()) {
				msg = "line name can't be null";
			}
			bool found = false;
			for (auto& l : data.lines) {
				if (&l == line) continue;
				if (l.name == changeLineName) {
					msg = "line name already exists";
					found = true;
					break;
				}
			}
			if (!found) {
				selectedLineName = line->name = changeLineName;
			}
		}


		ImGui::Checkbox("loop", &line->isLoop);
		ImGui::SameLine({}, 28);
		if (ImGui::Button("new point")) {
			line->points.emplace_back();
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

		ImGui::Text("p");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afPosx", &afPos.x, {}, {}, "%.0f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afPosy", &afPos.y, {}, {}, "%.0f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::Text("s");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afScalex", &afScale.x, {}, {}, "%.2f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afScaley", &afScale.y, {}, {}, "%.2f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::Text("r");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(65);
		ImGui::InputFloat("##afAngle", &afAngle, {}, {}, "%.2f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		if (ImGui::Button("go")) {
			auto&& at = xx::AffineTransform::MakePosScaleRadians(afPos, afScale, afAngle * M_PI / 360.f);
			for (auto& p : line->points) {
				auto&& pos = at.Apply(xx::XY{ (float)p.x, (float)p.y });
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
	auto&& h = (xx::engine.h - margin * 3 - (leftCmdPanelHeight1 + leftCmdPanelHeight2)) / 2;
	p.y += margin * 2 + leftCmdPanelHeight1 + leftCmdPanelHeight2 + h;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, h));
	ImGui::Begin("points", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	if (selectedLineName.size()) {
		auto&& line = GetLineByName(selectedLineName);
		assert(line);

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
			for (int e = line->points.size(); rowId < e; ++rowId) {
				auto& p = line->points[rowId];

				ImGui::TableNextRow();
				ImGui::PushStyleColor(ImGuiCol_Button, selectedPointIdex == rowId ? pressColor : normalColor);

				int n = 0;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				if (ImGui::Button(">")) {
					selectedPointIdex = rowId;
				}
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				if (ImGui::Button("+")) {
					line->points.insert(line->points.begin() + rowId, MovePathStore::Point{});
					selectedPointIdex = -1;
				}
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputInt("##", &p.x, 0, 0, ImGuiInputTextFlags_CharsDecimal);
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputInt("##", &p.y, 0, 0, ImGuiInputTextFlags_CharsDecimal);
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputInt("##", &p.tension, 0, 0, ImGuiInputTextFlags_CharsDecimal);
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputInt("##", &p.numSegments, 0, 0, ImGuiInputTextFlags_CharsDecimal);
				ImGui::PopID();

				++n;
				ImGui::TableSetColumnIndex(n);
				ImGui::PushID(rowId * numCols + n);
				if (ImGui::Button("X")) {
					removeRowId = rowId;
					selectedPointIdex = -1;
				}
				ImGui::PopID();

				ImGui::PopStyleColor(1);
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

	if (!ImGui::IsAnyItemActive()) {

		if (xx::engine.Pressed(xx::KbdKeys::W) && KeyboardGCDCheck()) {
			if (selectedLineName.empty()) {
				if (data.lines.size()) {
					SelectLine(data.lines[0].name);
				}
			} else {
				auto&& i = GetSelectedLineIndex();
				assert(i != -1);
				if (i > 0) {
					SelectLine(data.lines[i - 1].name);
				}
			}
			ClearEditState();
		}

		if (xx::engine.Pressed(xx::KbdKeys::S) && KeyboardGCDCheck()) {
			if (selectedLineName.empty()) {
				if (data.lines.size()) {
					SelectLine(data.lines[0].name);
				}
			} else {
				auto&& i = GetSelectedLineIndex();
				assert(i != -1);
				if (i < (int)data.lines.size() - 1) {
					SelectLine(data.lines[i + 1].name);
				}
			}
			ClearEditState();
		}

		if (xx::engine.Pressed(xx::KbdKeys::Up) && KeyboardGCDCheck()) {
			if (selectedLineName.size()) {
				if (auto&& i = GetSelectedLineIndex(); i > 0) {
					std::swap(data.lines[i], data.lines[i - 1]);
				}
			}
		}

		if (xx::engine.Pressed(xx::KbdKeys::Down) && KeyboardGCDCheck()) {
			if (selectedLineName.size()) {
				if (auto&& i = GetSelectedLineIndex(); i < (int)data.lines.size() - 1) {
					std::swap(data.lines[i], data.lines[i + 1]);
				}
			}
		}

	}

	auto&& line = GetSelectedLine();

	if (!ImGui::IsAnyItemActive() &&
		xx::engine.mousePosition.x > -xx::engine.w / 2 + leftPanelWidth + margin) {		// limit mouse pos in  edit area

		if (xx::engine.Pressed(xx::KbdKeys::Z) && KeyboardGCDCheck()) {
			zoom += 0.05;
			if (zoom >= 2) {
				zoom = 2;
			}
		}

		if (xx::engine.Pressed(xx::KbdKeys::X) && KeyboardGCDCheck()) {
			zoom -= 0.05;
			if (zoom <= 0.2) {
				zoom = 0.2;
			}
		}

		if (xx::engine.Pressed(xx::KbdKeys::A) && KeyboardGCDCheck()) {
			if (line) {
				auto&& xy = (xx::engine.mousePosition - offset) / zoom;
				auto& p = line->points.emplace_back();
				p.x = xy.x;
				p.y = xy.y;
			}
		}

		if (xx::engine.Pressed(xx::KbdKeys::D) && KeyboardGCDCheck()) {
			if (line && line->points.size()) {
				if (selectedPointIdex >= 0) {
					line->points.erase(line->points.begin() + selectedPointIdex);
					selectedPointIdex = -1;
				} else {
					line->points.pop_back();
					if (selectedPointIdex >= (int)line->points.size()) {
						selectedPointIdex = -1;
					}
				}
			}
		}
	}

	if (!line) return 0;

	meListener.Update();
	dc.looper = this;
	int i = 0;
	while (meListener.eventId && i < line->points.size()) {
		dc.point = &line->points[i];
		dc.idx = i;
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
	for (int i = 0, e = line->points.size(); i < e; ++i) {
		auto& p = line->points[i];
		xx::XY pos{ (float)p.x, (float)p.y };
		cps.emplace_back(pos, (float)p.tension / 100.f, (int32_t)p.numSegments);
		lsPoint.SetPosition(pos * zoom + offset).SetColor(i == selectedPointIdex ? xx::RGBA8{ 255,0,0,255 } : i == 0 ? xx::RGBA8{ 0,255,255,255 } : xx::RGBA8{ 255,255,255,255 }).Draw();
	}
	if (line->points.size() < 2) return 0;
	mp.Clear();
	mp.FillCurve(line->isLoop, cps);

	// draw body segments
	if (mp.totalDistance > 1.f) {
		xx::MovePathCache mpc;
		mpc.Init(mp);

		auto& ps = ls.SetColor({ 255,255,255,255 }).SetPosition(offset).SetScale(zoom).Clear().SetPoints();
		for (auto& p : mpc.points) {
			ps.emplace_back(p.pos);
			if (ps.size() == 65535) {		// batch commit
				ls.Draw();
				ls.Clear();
				ps.emplace_back(p.pos);
			}
		}
		ls.Draw();
	}

	xx::SimpleLabel lbl;
	lbl.SetAnchor({ 0,0 }).SetColor({ 127,127,0,255 })
		.SetPosition(xx::engine.ninePoints[1].MakeAdd(leftPanelWidth + margin * 2, 32 + margin))
		.SetText(fnt, xx::ToString("Arrow Up/Down: move line   zoom = ", zoom, ", points.size() = ", line->points.size()))
		.Draw()
		.SetAnchor({ 0,1 })
		.SetPosition(xx::engine.ninePoints[7].MakeAdd(leftPanelWidth + margin * 2, -margin))
		.SetText(fnt, "Z/X: Zooom in/out  W/S: select prev/next line  A/D: add/remove selected or last point MB1: select & drag point"sv, 32
			, xx::engine.w - (leftPanelWidth + margin * 3))
		.Draw();
	return 0;
}

void GameLooper::SelectLine(std::string_view const& name) {
	if (selectedLineName == name) return;
	selectedLineName = name;
	if (selectedLineName.empty()) return;
	changeLineName = name;
	ClearEditState();
}

int GameLooper::GetLineIndexByName(std::string_view const& name) {
	for (size_t i = 0, e = data.lines.size(); i < e; i++) {
		if (name == data.lines[i].name) return i;
	}
	return -1;
}

int GameLooper::GetSelectedLineIndex() {
	return GetLineIndexByName(selectedLineName);
}
MovePathStore::Line* GameLooper::GetLineByName(std::string_view const& name) {
	for (size_t i = 0, e = data.lines.size(); i < e; i++) {
		if (name == data.lines[i].name) return &data.lines[i];
	}
	return {};
}
MovePathStore::Line* GameLooper::GetSelectedLine() {
	return GetLineByName(selectedLineName);
}

bool GameLooper::CreateNewLine() {
	if (newLineName.empty()) {
		msg = "line name can't be null";
		return false;
	}
	bool found = false;
	for (auto& l : data.lines) {
		if (l.name == newLineName) {
			msg = "line name already exists";
			return false;
		}
	}
	data.lines.emplace_back().name = newLineName;
	MakeNewLineName();
	return true;
}

void GameLooper::MakeNewLineName() {
	if (data.lines.empty()) {
		newLineName = "c1";
	} else {
		newLineName.clear();
		int v{};

		auto&& n = data.lines.back().name;
		int i, e = (int)n.size() - 1;
		for (i = e; i >= 0; --i) {
			auto& c = n[i];
			if (c < '0' || c > '9') break;
		}
		if (i == -1) {			// n is number
			xx::SvToNumber(n, v);
			xx::Append(newLineName, v + 1);
		} else if (i == e) {	// suffix has not number
			newLineName = n + "1";
		} else {
			xx::SvToNumber(std::string_view(n.data() + i + 1), v);
			xx::Append(newLineName, std::string_view(n.data(), i + 1), v + 1);
		}
	}
}


int GameLooper::KeyboardGCDCheck() {
	if (keyboardGCDNowSecs < xx::engine.nowSecs) return SetKeyboardGCD();
	return 0;
}
int GameLooper::SetKeyboardGCD() {
	keyboardGCDNowSecs = xx::engine.nowSecs + keyboardGCD;
	return 1;
}

void GameLooper::ClearEditState() {
	selectedPointIdex = -1;
	dc.idx = -1;
	meListener.handler = {};
}

bool DragableCircle::HandleMouseDown(DragableCircleMouseEventListener& L) {
	if (idx == -1) return false;
	auto&& dx = looper->offset.x + point->x * looper->zoom - L.downPos.x;
	auto&& dy = looper->offset.y + point->y * looper->zoom - L.downPos.y;
	if (dx * dx + dy * dy < GameLooper::pointRadius * GameLooper::pointRadius) {
		pos = xx::XY{ (float)point->x, (float)point->y };
		looper->selectedPointIdex = idx;
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
