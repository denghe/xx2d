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

	if (contentViewer) {
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
	if (Convertable()) {
		ImGui::SameLine();
		if (ImGui::Button(xx::ToString("convert to ", genNamePrefix + "__??fps_??k__.webm").c_str())) {
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
	contentViewer.Reset();

	std::u8string_view res((std::u8string&)resPath);
	if (!std::filesystem::exists(res) || !std::filesystem::is_directory(res)) {
		FillDefaultResPath();
	} else {
		resPath = (std::string&)std::filesystem::absolute(res).u8string();
		if (resPath.back() != '\\' && resPath.back() != '/') {
			resPath += '\\';
		}
	}
	res = std::u8string_view((std::u8string&)resPath);

	std::u8string_view prefix((std::u8string&)fileNamePrefix);
	files.clear();
	for (auto&& o : std::filesystem::directory_iterator(res)) {
		if (!o.is_regular_file()) continue;
		auto oStr = std::filesystem::absolute(o).u8string();
		auto subStr = std::u8string_view(oStr).substr(res.size());
		if (subStr.starts_with(prefix)) {
			files.push_back((std::string&)oStr.substr(res.size()));
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
	genNamePrefix = GetSameName(fs);
	if (genNamePrefix.empty()) return false;

	// remove " (" num ")"
	while (genNamePrefix.ends_with('(')) {
		genNamePrefix = genNamePrefix.substr(0, genNamePrefix.size() - 1);
	}
	while (genNamePrefix.ends_with(' ')) {
		genNamePrefix = genNamePrefix.substr(0, genNamePrefix.size() - 1);
	}
	if (genNamePrefix.empty()) return false;

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

bool GameLooper::FindFFMpegExe() {
	auto lines = std::move(raymii::Command::exec("where ffmpeg.exe").output);
	if (lines.size()) {
		std::string_view line(lines);
		ffmpegPath = xx::Trim(xx::SplitOnce(line, "\n"));
	}
	return ffmpegPath.size();
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
	genNamePrefix = GetSameName(fs);
	assert(genNamePrefix.size());
	// remove " (" num ")"
	while (genNamePrefix.ends_with('(')) {
		genNamePrefix = genNamePrefix.substr(0, genNamePrefix.size() - 1);
	}
	while (genNamePrefix.ends_with(' ')) {
		genNamePrefix = genNamePrefix.substr(0, genNamePrefix.size() - 1);
	}
	if (genNamePrefix.empty()) return;

	// set curr dir
	//SetCurrentDirectoryA(resPath.c_str());
	std::filesystem::current_path((std::u8string&)resPath);

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
			args += genNamePrefix;
			args += "__" + std::to_string(fps) + "fps_" + rs + "k__.webm";

			PopupConsole(ffmpegPath.c_str(), (LPSTR)args.c_str());
		}
	} else {

		for (auto& r : rates) {
			auto rs = std::to_string(r);

			// cmd: ffmpeg.exe -f image2 -framerate 1 -i "??????" -c:v libvpx-vp9 -pix_fmt yuva420p -b:v ?????K -speed 0 ??????.webm
			std::string args = " -f image2 -framerate 1 -i \"" + std::string(fs[0]) + "\" -c:v libvpx-vp9 -pix_fmt yuva420p -b:v " + rs + "K -speed 0 ";
			args += genNamePrefix;
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
	if (contentViewer) {
		contentViewer->ZoomOut();
	}
}

void GameLooper::ZoomIn() {
	if (contentViewer) {
		contentViewer->ZoomIn();
	}
}



// png, pkm2, astc
struct ContentViewer_Pic : ContentViewerBase {
	xx::Quad quad;
	std::string info, info2;
	float zoom{1.f};
	void Init(GameLooper* looper_, std::string_view const& buf, std::string fullPath, std::string info_) {
		looper = looper_;
		info = std::move(info_);

		auto&& tex = xx::Make<xx::GLTexture>(xx::LoadGLTexture(buf, fullPath));	// todo: try ?

		// auto fill? calc zoom?

		quad.SetTexture(tex).SetScale(zoom);

		xx::Append(info2, ", width = ", quad.Size().x, ", height = ", quad.Size().y);
	}
	void Update() override {
		xx::XY pos{ (GameLooper::leftPanelWidth + GameLooper::margin * 2) / 2.f, -(GameLooper::rightTopPanelHeight + GameLooper::margin * 2) / 2.f };
		quad.SetPosition(pos).Draw();

		pos = xx::engine.ninePoints[7] + xx::XY{ GameLooper::leftPanelWidth + GameLooper::margin * 2, -(GameLooper::rightTopPanelHeight + GameLooper::margin * 2) };
		auto limitWidth = xx::engine.w - (GameLooper::leftPanelWidth + GameLooper::margin * 3);
		xx::SimpleLabel().SetPosition(pos).SetAnchor({0,1}).SetText(looper->fnt, info + info2, 32.f, limitWidth).Draw();
	}
	void ZoomIn() override {
		if (zoom > 0.1f) {
			zoom -= 0.1f;
		} else {
			zoom = 0.1f;
		}
		quad.SetScale(zoom);
	}
	void ZoomOut() override {
		if (zoom < 10.f) {
			zoom += 0.1f;
		} else {
			zoom = 10.f;
		}
		quad.SetScale(zoom);
	}
};

// webm, xxmv
struct ContentViewer_Webm : ContentViewerBase {
	xx::Quad quad;
	xx::List<xx::Shared<xx::GLTexture>> texs;
	xx::Mv mv;
	std::string info, info2;
	float zoom{1.f};
	int cursor{};
	float timePool{}, delay{};

	void Init(GameLooper* looper_, std::string_view const& buf, std::string fullPath, std::string info_) {
		looper = looper_;
		info = std::move(info_);

		int r = mv.Load(xx::Data_r(buf.data(), buf.size()));
		// todo: r

		auto&& shader = xx::engine.sm.GetShader<xx::Shader_Yuva2Rgba>();
		mv.ForeachFrame([&](int const& frameIndex, uint32_t const& w, uint32_t const& h
			, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData, uint32_t const& yaStride, uint32_t const& uvStride)->int {

				auto tex = xx::FrameBuffer().Init().Draw({ w, h }, true, xx::RGBA8{}, [&]() {
					shader.Draw(yData, uData, vData, aData, yaStride, uvStride, w, h, {});
					});

				texs.Add(tex);

				return 0;
			});

		quad.SetTexture(texs[cursor]).SetScale(zoom);

		delay = mv.duration / (float)mv.count / 1000.f;
		
		xx::Append(info2, ", width = ", mv.width, ", height = ", mv.height, ", count = ", mv.count, ", duration = ", mv.duration);
	}
	void Update() override {
		timePool += xx::engine.delta;
		while (timePool >= delay) {
			timePool -= delay;

			if (++cursor == texs.len) {
				cursor = 0;
			}
			quad.SetTexture(texs[cursor]);
		}

		xx::XY pos{ (GameLooper::leftPanelWidth + GameLooper::margin * 2) / 2.f, -(GameLooper::rightTopPanelHeight + GameLooper::margin * 2) / 2.f };
		quad.SetPosition(pos).Draw();

		pos = xx::engine.ninePoints[7] + xx::XY{ GameLooper::leftPanelWidth + GameLooper::margin * 2, -(GameLooper::rightTopPanelHeight + GameLooper::margin * 2) };
		auto limitWidth = xx::engine.w - (GameLooper::leftPanelWidth + GameLooper::margin * 3);
		xx::SimpleLabel().SetPosition(pos).SetAnchor({0,1}).SetText(looper->fnt, info + info2, 32.f, limitWidth).Draw();
	}
	void ZoomIn() override {
		if (zoom > 0.1f) {
			zoom -= 0.1f;
		} else {
			zoom = 0.1f;
		}
		quad.SetScale(zoom);
	}
	void ZoomOut() override {
		if (zoom < 10.f) {
			zoom += 0.1f;
		} else {
			zoom = 10.f;
		}
		quad.SetScale(zoom);
	}
};


void GameLooper::DrawSelectedFile() {
	auto fullPath = std::filesystem::path((std::u8string&)resPath) / (std::u8string&)selectedFile;

	std::string info;
	xx::Data d;
	int r = xx::ReadAllBytes(fullPath, d);
	if (r) {
		msg = xx::ToString("read ", fullPath, " error! r = ", r);
		return;
	}
	std::string_view buf(d);

	if (buf.starts_with("\x28\xB5\x2F\xFD"sv)) {	// zstd
		xx::Append(info, "zstd file size = ", d.len);
		xx::Data d2;
		ZstdDecompress(d, d2);
		d = std::move(d2);
		buf = d;
		xx::Append(info, ", decompress size = ", d.len);
	} else {
		xx::Append(info, "file size = ", d.len);
	}

	// check d header & switch file type
	if (buf.starts_with("\x1a\x45\xdf\xa3"sv)) {
		xx::Append(info, ", content type: webm");
		auto v = xx::Make<ContentViewer_Webm>();
		contentViewer = v;
		v->Init(this, buf, (std::string&)fullPath.u8string(), std::move(info));
	} else if (buf.starts_with("XXMV 1.0"sv)) {
		xx::Append(info, ", content type: xxmv");
		auto v = xx::Make<ContentViewer_Webm>();
		contentViewer = v;
		v->Init(this, buf, (std::string&)fullPath.u8string(), std::move(info));
	} else if (buf.starts_with("PKM 20"sv)) {
		xx::Append(info, ", content type: pkm2");
		auto v = xx::Make<ContentViewer_Pic>();
		contentViewer = v;
		v->Init(this, buf, (std::string&)fullPath.u8string(), std::move(info));
	} else if (buf.starts_with("\x13\xab\xa1\x5c"sv)) {
		xx::Append(info, ", content type: astc");
		auto v = xx::Make<ContentViewer_Pic>();
		contentViewer = v;
		v->Init(this, buf, (std::string&)fullPath.u8string(), std::move(info));
	} else if (buf.starts_with("\x89\x50\x4e\x47\x0d\x0a\x1a\x0a"sv)) {
		xx::Append(info, ", content type: png");
		auto v = xx::Make<ContentViewer_Pic>();
		contentViewer = v;
		v->Init(this, buf, (std::string&)fullPath.u8string(), std::move(info));
	} else {
		contentViewer.Reset();
	}
}
