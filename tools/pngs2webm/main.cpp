#include "pch.h"
#include "main.h"
#include <imgui_stdlib.h>
#include <command.h>
#include <ranges>

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


void GameLooper::Init() {

	xx::engine.imguiInit = [] { 
		auto&& io = ImGui::GetIO();
		io.Fonts->ClearFonts();
		auto&& imfnt = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 24, {}, io.Fonts->GetGlyphRangesChineseFull());
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

	if (!FindFFMpegExe()) {
		msg = "can't find ffmpeg.exe! please set system environt PATH & restart tool!";
	}

	FillDefaultResPath();
}

int GameLooper::Update() {
	offset = { (leftPanelWidth + margin * 2) / 2, -(rightTopPanelHeight + margin * 2) / 2 };

	if (!ImGui::IsAnyItemActive()) {
		if (xx::engine.Pressed(xx::KbdKeys::W) && KeyboardGCDCheck()) MoveUp();
		if (xx::engine.Pressed(xx::KbdKeys::S) && KeyboardGCDCheck()) MoveDown();
		if (xx::engine.Pressed(xx::KbdKeys::A) && KeyboardGCDCheck()) MoveTop();
		if (xx::engine.Pressed(xx::KbdKeys::D) && KeyboardGCDCheck()) MoveBottom();
		if (xx::engine.Pressed(xx::KbdKeys::Z) && KeyboardGCDCheck()) ZoomOut();
		if (xx::engine.Pressed(xx::KbdKeys::X) && KeyboardGCDCheck()) ZoomIn();
	}

	if (contentViewer.has_value()) {
		contentViewer->Update();
	}
	
	fpsViewer.Update();
	return 0;
}

void GameLooper::ImGuiUpdate() {

	if (msg.has_value()) {
		ImGuiDrawWindow_Error();
		return;
	}

	ImGuiDrawWindow_Left();
	ImGuiDrawWindow_RightTop();
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

		int rowId{}, removeRowId{ -1 };
		for (auto& f : files) {

			ImGui::TableNextRow();

			int n = 0;

			ImGui::TableSetColumnIndex(n);
			ImGui::PushID(rowId * numCols + n);
			ImGui::PushStyleColor(ImGuiCol_Button, f == selectedFile ? pressColor : normalColor);
			auto&& sg = xx::MakeScopeGuard([] { ImGui::PopStyleColor(1); });
			if (ImGui::Button("==>")) {
				selectedFile = f;
				DrawSelectedFile();
			}
			ImGui::PopID();

			++n;
			ImGui::TableSetColumnIndex(n);
			ImGui::PushID(rowId * numCols + n);
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::Text(f.c_str());
			ImGui::PopID();

			++rowId;
		}
		ImGui::EndTable();
	}

	ImGui::End();
}


void GameLooper::ImGuiDrawWindow_RightTop() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin + leftPanelWidth;
	p.y += margin;
	auto&& w = xx::engine.w - margin * 2 - leftPanelWidth;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(w, rightTopPanelHeight));
	ImGui::Begin("controlpanel", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	ImGui::Text("ffmpeg.exe path：");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("##ffmpeg.exe path：", &ffmpegPath);

	ImGui::Text("root dir：");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("##root dir：", &resPath);

	if (ImGui::Button("refresh")) {
		ReloadFiles();
	}
	ImGui::SameLine();
	ImGui::Text("filename prefix filter：");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputText("##filename prefix filter：", &fileNamePrefix);

	if (Convertable()) {
		ImGui::Text("rate[, rate ...]：");
		ImGui::SameLine();
		ImGui::PushItemWidth(300);
		ImGui::InputText("##rate[, rate ...]：", &ratesString);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("fps：");
		ImGui::SameLine();
		ImGui::PushItemWidth(120);
		ImGui::InputInt("##fps：", &fps, 15);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("call FFMPEG.EXE to convert current pic list to webm ( every rate )")) {
			ConvertFiles();
		}
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


void GameLooper::ReloadFiles() {
	selectedFile.clear();
	contentViewer.reset();

	std::string_view res(resPath);
	if (!std::filesystem::exists(res) || !std::filesystem::is_directory(res)) {
		FillDefaultResPath();
		res = std::string_view(resPath);
	} else {
		resPath = std::filesystem::absolute(res).string();
		if (resPath.back() != '\\' && resPath.back() != '/') {
			resPath += '\\';
		}
		res = std::string_view(resPath);
	}

	auto len = strlen(fileNamePrefix.data());
	std::string_view prefix(fileNamePrefix.data(), len);
	files.clear();
	for (auto&& o : std::filesystem::directory_iterator(res)) {
		if (!o.is_regular_file()) continue;
		auto oStr = std::filesystem::absolute(o).string();
		auto subStr = std::string_view(oStr).substr(res.size());
		if (subStr.starts_with(prefix)) {
			files.push_back(oStr.substr(res.size()));
		}
	}

	// sort by name with number
	std::sort(files.begin(), files.end(), [](std::string const& a, std::string const& b)->bool {
		return xx::InnerNumberToFixed(a) < xx::InnerNumberToFixed(b);
		});
}

void GameLooper::FillDefaultResPath() {
	resPath = std::filesystem::absolute("res").string();
	if (resPath.back() != '\\' && resPath.back() != '/') {
		resPath += '\\';
	}
}

bool GameLooper::Convertable() {
	auto fs = GetPicFiles();
	if (fs.empty()) return false;
	auto name = GetSameName(fs);
	if (name.empty()) return false;

	// todo: check fs: real type == .ext

	FillRates();
	return !ratesString.empty();
}

std::vector<std::string_view> GameLooper::GetPicFiles() {
	std::vector<std::string_view> picFiles;
	for (auto& f : files) {
		if (IsPic(f)) {
			picFiles.push_back(f);
		}
	}
	return picFiles;
}

bool GameLooper::IsPic(std::string_view sv) {
	if (sv.ends_with(".png"sv)) return true;
	else if (sv.ends_with(".jpg"sv)) return true;
	else if (sv.ends_with(".webp"sv)) return true;
	return false;
}

void ContentViewer::Init(GameLooper* looper_) {
	looper = looper_;
	// todo: draw selected file
	// fill selectedFileInfo
	assert(looper->selectedFile.size());

	// todo: check file name, get real type, draw ?
}

void ContentViewer::Update() {
	// todo
	//previewArea->removeAllChildrenWithCleanup(true);
	//selectedFileInfo.clear();
	//if (selectedFile.empty()) return;

	//auto fullPath = std::filesystem::path(resPath.c_str()) / selectedFile;
	//auto fileSize = std::filesystem::file_size(fullPath) / 1000;
	//zoom = (float)std::strtod(zoomString.data(), nullptr);
	//if (zoom < 0.001f) {
	//	zoom = 0.001f;
	//}
	//if (zoom > 100.f) {
	//	zoom = 100.f;
	//}
	//zoomString = std::to_string(zoom);

	//if (IsPic(selectedFile) || selectedFile.ends_with(".pkm"sv) || selectedFile.ends_with(".pkm.gz"sv)) {
	//	auto sprite = cocos2d::Sprite::create(fullPath.string());
	//	previewArea->addChild(sprite);
	//	sprite->setScale(zoom);
	//	auto wh = sprite->getTexture()->getContentSize();
	//	xx::Append(selectedFileInfo, "file size = ", fileSize, "k, width = ", (int)wh.width, ", height = ", (int)wh.height);

	//} else if (selectedFile.ends_with(".webm"sv)) {
	//	int r = mv.LoadFromWebm(fullPath);
	//	if (r) return;
	//	cocos2d::SpriteFrameCache::getInstance()->removeSpriteFrames();
	//	r = mv.FillToSpriteFrameCache(selectedFile, "_", "");
	//	if (r) return;

	//	cocos2d::Vector<cocos2d::SpriteFrame*> sfs;
	//	auto sfc = cocos2d::SpriteFrameCache::getInstance();
	//	for (uint32_t i = 1; i <= mv.count; ++i) {
	//		auto sf = sfc->getSpriteFrameByName(selectedFile + "_" + std::to_string(i));
	//		assert(sf);
	//		sfs.pushBack(sf);
	//	}
	//	auto a = cocos2d::Animation::createWithSpriteFrames(sfs, mv.duration / 1000.f / mv.count);

	//	auto sprite = cocos2d::Sprite::create();
	//	previewArea->addChild(sprite);
	//	sprite->setScale(zoom);
	//	sprite->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(a)));

	//	xx::Append(selectedFileInfo, "file size = ", fileSize, "k, width = ", mv.width, ", height = ", mv.height, ", count = ", mv.count, ", duration = ", (int)mv.duration);
	//}
}

void ContentViewer::ZoomIn() {
	// todo
}

void ContentViewer::ZoomOut() {
	// todo
}



bool GameLooper::FindFFMpegExe() {
	auto lines = std::move(raymii::Command::exec("where ffmpeg.exe").output);
	if (lines.size()) {
		std::string_view line(lines);
		ffmpegPath = xx::Trim(xx::SplitOnce(line, "\n"));
	}
	return ffmpegPath.size();
}

void GameLooper::DrawSelectedFile() {
	contentViewer.emplace();
	contentViewer->Init(this);
}


#define WIN32_LEAN_AND_MEAN
#include <process.h>
#include <windows.h>
inline static int PopupConsole(LPCSTR exePath, LPSTR cmdLineArgs) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	if (!CreateProcessA(
		exePath,
		cmdLineArgs,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi
	)) {
		auto e = GetLastError();
		CloseHandle(pi.hThread);
		return __LINE__;
	}
	return 0;
}

void GameLooper::ConvertFiles() {
	auto fs = GetPicFiles();
	assert(!fs.empty());
	auto name = GetSameName(fs);
	assert(name.size());
	// remove " (" num ")"
	while (name.ends_with('(')) {
		name = name.substr(0, name.size() - 1);
	}
	while (name.ends_with(' ')) {
		name = name.substr(0, name.size() - 1);
	}
	if (name.empty()) return;

	// set curr dir
	SetCurrentDirectoryA(resPath.c_str());

	if (fs.size() > 1) {

		// append ffmpeg_input.txt
		std::string txt;
		for (auto& f : fs) {
			txt += "file '";
			txt += f;
			txt += "'\r\nduration " + std::to_string(1. / fps) + "\r\n";
		}
		txt += "file '";
		txt += fs.back();
		txt += "'";
		std::string inputFN = "ffmpeg_input.txt";
		xx::WriteAllBytes(inputFN, txt.c_str(), txt.size());

		// append ffmpeg cmd line for every rate
		// cmd: ffmpeg.exe -f concat -safe 0 -i ffmpeg_input.txt -c:v libvpx-vp9 -pix_fmt yuva420p -b:v ?????K -speed 0 ??????.webm
		for (auto& r : rates) {
			auto rs = std::to_string(r);

			std::string args = " -f concat -safe 0 -i " + inputFN + " -c:v libvpx-vp9 -pix_fmt yuva420p -b:v " + rs + "K -speed 0 ";
			args += name;
			args += "__" + std::to_string(fps) + "fps_" + rs + "k__.webm";

			PopupConsole(ffmpegPath.c_str(), (LPSTR)args.c_str());
		}
	} else {

		for (auto& r : rates) {
			auto rs = std::to_string(r);

			// cmd: ffmpeg.exe -f image2 -framerate 1 -i "??????" -c:v libvpx-vp9 -pix_fmt yuva420p -b:v ?????K -speed 0 ??????.webm
			std::string args = " -f image2 -framerate 1 -i \"" + std::string(fs[0]) + "\" -c:v libvpx-vp9 -pix_fmt yuva420p -b:v " + rs + "K -speed 0 ";
			args += name;
			args += "__" + rs + "k__.webm";

			PopupConsole(ffmpegPath.c_str(), (LPSTR)args.c_str());
		}
	}
}

std::string_view GameLooper::GetSameName(std::vector<std::string_view> const& fs) {
	if (fs.empty()) return "";
	// find shortest
	auto len = std::numeric_limits<size_t>::max();
	for (auto& f : fs) {
		if (len > f.size()) {
			len = f.size();
		}
	}
	// foreach find diff
	for (size_t i = 0; i < len; i++) {
		char c = fs[0][i];
		for (auto& f : fs) {
			if (f[i] != c) {
				return fs[0].substr(0, i);
			}
		}
	}
	return fs[0].substr(0, len);
}

void GameLooper::FillRates() {
	// filter ratesString
	rates.clear();
	std::string s;
	std::string_view sv(ratesString.data());
	std::ranges::for_each(sv | std::views::split(','), [&](auto const& v) {
		if (v.empty()) return;
		s.clear();
		for (auto& c : v) {
			if (c >= '0' && c <= '9') {
				s += c;
			}
		}
		if (s.size()) {
			rates.insert(std::strtol(s.c_str(), nullptr, 10));
		}
		});
	ratesString.clear();
	for (auto& r : rates) {
		ratesString.append(std::to_string(r));
		ratesString += ',';
	}
	if (ratesString.size()) {
		ratesString.pop_back();
	}
}


void GameLooper::MoveUp() {
	if (selectedFile.empty()) {
		MoveBottom();
		return;
	}
	auto iter = std::find(files.begin(), files.end(), selectedFile);
	if (iter != files.begin()) {
		selectedFile = *(--iter);
		DrawSelectedFile();
	}
}

void GameLooper::MoveDown() {
	if (selectedFile.empty()) {
		MoveTop();
		return;
	}
	auto iter = std::find(files.begin(), files.end(), selectedFile);
	if (++iter == files.end()) return;
	selectedFile = *iter;
	DrawSelectedFile();
}

void GameLooper::MoveBottom() {
	if (files.empty()) return;
	selectedFile = files.back();
	DrawSelectedFile();
}

void GameLooper::MoveTop() {
	if (files.empty()) return;
	selectedFile = files.front();
	DrawSelectedFile();
}

void GameLooper::ZoomOut() {
	if (contentViewer.has_value()) {
		contentViewer->ZoomOut();
	}
}

void GameLooper::ZoomIn() {
	if (contentViewer.has_value()) {
		contentViewer->ZoomIn();
	}
}
