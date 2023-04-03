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

	// todo: right button content draw?
	
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
	ImGuiDrawWindow_RightBottom();
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


void GameLooper::ImGuiDrawWindow_RightTop() {
	ImVec2 p = ImGui::GetMainViewport()->Pos;
	p.x += margin + leftPanelWidth;
	p.y += margin;
	auto&& w = xx::engine.w - margin * 2 - leftPanelWidth;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(w, rightTopPanelHeight));
	ImGui::Begin("controlpanel", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	ImGui::End();
}

void GameLooper::ImGuiDrawWindow_RightBottom() {

}


int GameLooper::KeyboardGCDCheck() {
	if (keyboardGCDNowSecs < xx::engine.nowSecs) return SetKeyboardGCD();
	return 0;
}
int GameLooper::SetKeyboardGCD() {
	keyboardGCDNowSecs = xx::engine.nowSecs + keyboardGCD;
	return 1;
}

void GameLooper::MoveUp() {

}
void GameLooper::MoveDown() {

}
void GameLooper::MoveTop() {

}
void GameLooper::MoveBottom() {

}
void GameLooper::ZoomOut() {

}
void GameLooper::ZoomIn() {

}


/*


bool HelloWorld::init() {
	if (!this->BaseType::init()) return false;

	DW = AppDelegate::designWidth;
	DH = AppDelegate::designHeight;
	DW_2 = DW / 2;
	DH_2 = DH / 2;

	CX = MARGIN + FILE_LIST_WIDTH + (DW - MARGIN - FILE_LIST_WIDTH) / 2;
	CY = (DH - MARGIN - CONTROL_PANEL_HEIGHT) / 2;

	previewArea = cocos2d::Node::create();
	addChild(previewArea);
	previewArea->setPosition(CX, CY);

	fileNamePrefix.resize(1024);
	ratesString.resize(1024);
	resPath.resize(1024);
	fpsString.resize(1024);
	zoomString.resize(1024);

	fpsString = "30";
	ratesString = "10,20,40,60,80,100,150,200,300";

	zoom = 1;
	zoomString = std::to_string(zoom);

	FillDefaultResPath();

	auto kbListener = cocos2d::EventListenerKeyboard::create();
	kbListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
		switch (keyCode) {
			case cocos2d::EventKeyboard::KeyCode::KEY_W:
				MoveUp();
				return;
			case cocos2d::EventKeyboard::KeyCode::KEY_S:
				MoveDown();
				return;
			case cocos2d::EventKeyboard::KeyCode::KEY_D:
				MoveBottom();
				return;
			case cocos2d::EventKeyboard::KeyCode::KEY_A:
				MoveTop();
				return;
			case cocos2d::EventKeyboard::KeyCode::KEY_Z:
				ZoomOut();
				return;
			case cocos2d::EventKeyboard::KeyCode::KEY_X:
				ZoomIn();
				return;
		}
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(kbListener, this);

	scheduleUpdate();
	return true;
}

void HelloWorld::onEnter() {
	this->BaseType::onEnter();
	//...
	cocos2d::extension::ImGuiPresenter::getInstance()->addFont("c:/windows/fonts/simhei.ttf", 16, cocos2d::extension::ImGuiPresenter::CHS_GLYPH_RANGE::FULL);
	cocos2d::extension::ImGuiPresenter::getInstance()->addRenderLoop("#test", CC_CALLBACK_0(HelloWorld::onDrawImGui, this), this);
}

void HelloWorld::onExit() {
	cocos2d::extension::ImGuiPresenter::getInstance()->removeRenderLoop("#test");
	cocos2d::extension::ImGuiPresenter::getInstance()->clearFonts();

	cocos2d::extension::ImGuiPresenter::destroyInstance();
	//...
	this->BaseType::onExit();
}

void HelloWorld::onDrawImGui() {
	ImGui::StyleColorsDark();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	auto sgStyleVar = xx::MakeScopeGuard([] { ImGui::PopStyleVar(1); });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, releaseColor);
	auto sgStyleColor = xx::MakeScopeGuard([] { ImGui::PopStyleColor(2); });
	posBegin = ImGui::GetMainViewport()->Pos;

	if (!ok) {
		DrawError();
		return;
	}
	DrawToolbar();
	DrawFilesList();
}

void HelloWorld::DrawError() {
	auto p = posBegin;
	p.x += 300;
	p.y += 450;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(DW - 300 * 2, DH - 450 * 2));
	ImGui::Begin("错误", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);
	ImGui::Text(errorMessage.c_str());
	ImGui::End();
}

void HelloWorld::DrawToolbar() {
	auto p = posBegin;
	p.x += MARGIN + FILE_LIST_WIDTH + MARGIN;
	p.y += MARGIN;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(DW - (MARGIN + FILE_LIST_WIDTH + MARGIN + MARGIN), CONTROL_PANEL_HEIGHT));
	ImGui::Begin("控制面板", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	ImGui::Text("根目录：");
	ImGui::SameLine();
	ImGui::InputText("a", resPath.data(), 1024);

	if (ImGui::Button("刷新")) {
		ReloadFiles();
	}
	ImGui::SameLine();
	ImGui::Text("文件名前缀 过滤：");
	ImGui::SameLine();
	ImGui::InputText("c", fileNamePrefix.data(), 1024);

	ImGui::Text("码率(逗号间隔)：");
	ImGui::SameLine();
	ImGui::PushItemWidth(300);
	ImGui::InputText("d", ratesString.data(), 1024);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::Text("帧率：");
	ImGui::SameLine();
	ImGui::PushItemWidth(100);
	ImGui::InputText("e", fpsString.data(), 1024);
	ImGui::PopItemWidth();
	if (Convertable()) {
		ImGui::SameLine();
		if (ImGui::Button("用 C:\\ffmpeg.exe 将 当前列出的图片 按每一种码率, 分别 转为一个 webm")) {
			ConvertFiles();
		}
	}

	ImGui::Text("预览缩放比例：");
	ImGui::SameLine();
	ImGui::PushItemWidth(100);
	if (ImGui::InputText("b", zoomString.data(), 1024)) {
		DrawSelectedFile();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::Text(selectedFileInfo.c_str());
	ImGui::End();
}

void HelloWorld::DrawFilesList() {
	auto p = posBegin;
	p.x += MARGIN;
	p.y += MARGIN;
	ImGui::SetNextWindowPos(p);
	ImGui::SetNextWindowSize(ImVec2(FILE_LIST_WIDTH, DH - MARGIN - MARGIN));
	ImGui::Begin("文件列表", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	ImGui::BeginChild("文件列表Scrolling");
	for (auto& f : files) {
		ImGui::PushStyleColor(ImGuiCol_Button, f == selectedFile ? pressColor : normalColor);
		auto sg = xx::MakeScopeGuard([] { ImGui::PopStyleColor(1); });
		if (ImGui::Button(f.c_str())) {
			if (f != selectedFile) {
				selectedFile = f;
				DrawSelectedFile();
			}
		}
	}
	ImGui::EndChild();
	ImGui::End();
}

void HelloWorld::DrawSelectedFile() {
	previewArea->removeAllChildrenWithCleanup(true);
	selectedFileInfo.clear();
	if (selectedFile.empty()) return;

	auto fullPath = std::filesystem::path(resPath.c_str()) / selectedFile;
	auto fileSize = std::filesystem::file_size(fullPath) / 1000;
	zoom = (float)std::strtod(zoomString.data(), nullptr);
	if (zoom < 0.001f) {
		zoom = 0.001f;
	}
	if (zoom > 100.f) {
		zoom = 100.f;
	}
	zoomString = std::to_string(zoom);

	if (IsPic(selectedFile) || selectedFile.ends_with(".pkm"sv) || selectedFile.ends_with(".pkm.gz"sv)) {
		auto sprite = cocos2d::Sprite::create(fullPath.string());
		previewArea->addChild(sprite);
		sprite->setScale(zoom);
		auto wh = sprite->getTexture()->getContentSize();
		xx::Append(selectedFileInfo, "file size = ", fileSize, "k, width = ", (int)wh.width, ", height = ", (int)wh.height);

	} else if (selectedFile.ends_with(".webm"sv)) {
		int r = mv.LoadFromWebm(fullPath);
		if (r) return;
		cocos2d::SpriteFrameCache::getInstance()->removeSpriteFrames();
		r = mv.FillToSpriteFrameCache(selectedFile, "_", "");
		if (r) return;

		cocos2d::Vector<cocos2d::SpriteFrame*> sfs;
		auto sfc = cocos2d::SpriteFrameCache::getInstance();
		for (uint32_t i = 1; i <= mv.count; ++i) {
			auto sf = sfc->getSpriteFrameByName(selectedFile + "_" + std::to_string(i));
			assert(sf);
			sfs.pushBack(sf);
		}
		auto a = cocos2d::Animation::createWithSpriteFrames(sfs, mv.duration / 1000.f / mv.count);

		auto sprite = cocos2d::Sprite::create();
		previewArea->addChild(sprite);
		sprite->setScale(zoom);
		sprite->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(a)));

		xx::Append(selectedFileInfo, "file size = ", fileSize, "k, width = ", mv.width, ", height = ", mv.height, ", count = ", mv.count, ", duration = ", (int)mv.duration);
	}
}

void HelloWorld::ReloadFiles() {
	selectedFile.clear();
	previewArea->removeAllChildrenWithCleanup(true);

	auto res = std::string_view(resPath.data());
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

	// 排序( 如果含有数字，以其中的数字部分大小来排 )
	std::sort(files.begin(), files.end(), [](std::string const& a, std::string const& b)->bool {
		return xx::InnerNumberToFixed(a) < xx::InnerNumberToFixed(b);
		});
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

void HelloWorld::ConvertFiles() {
	auto fs = GetPicFiles();
	assert(!fs.empty());
	auto name = GetSameName(fs);
	assert(name.size());
	// 去掉一些图片名类似 xxxx (数字) 的括号和空格部分
	while (name.ends_with('(')) {
		name = name.substr(0, name.size() - 1);
	}
	while (name.ends_with(' ')) {
		name = name.substr(0, name.size() - 1);
	}
	if (name.empty()) return;

	// 设置当前目录
	SetCurrentDirectoryA(resPath.c_str());

	if (fs.size() > 1) {

		// 拼接 ffmpeg_input.txt
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

		// 拼接 ffmpeg 命令行，每种码率 弹一个窗执行
		// 原型: c:\ffmpeg.exe -f concat -safe 0 -i ffmpeg_input.txt -c:v libvpx-vp9 -pix_fmt yuva420p -b:v ?????K -speed 0 ??????.webm
		for (auto& r : rates) {
			auto rs = std::to_string(r);

			std::string args = " -f concat -safe 0 -i " + inputFN + " -c:v libvpx-vp9 -pix_fmt yuva420p -b:v " + rs + "K -speed 0 ";
			args += name;
			args += "__" + std::to_string(fps) + "fps_" + rs + "k__.webm";

			PopupConsole("C:\\ffmpeg.exe", (LPSTR)args.c_str());
		}
	} else {

		for (auto& r : rates) {
			auto rs = std::to_string(r);

			// 原型: c:\ffmpeg.exe -f image2 -framerate 1 -i "??????" -c:v libvpx-vp9 -pix_fmt yuva420p -b:v ?????K -speed 0 ??????.webm
			std::string args = " -f image2 -framerate 1 -i \"" + std::string(fs[0]) + "\" -c:v libvpx-vp9 -pix_fmt yuva420p -b:v " + rs + "K -speed 0 ";
			args += name;
			args += "__" + rs + "k__.webm";

			PopupConsole("C:\\ffmpeg.exe", (LPSTR)args.c_str());
		}
	}
}

bool HelloWorld::IsPic(std::string_view sv) {
	if (sv.ends_with(".png"sv)) return true;
	else if (sv.ends_with(".jpg"sv)) return true;
	else if (sv.ends_with(".webp"sv)) return true;
	return false;
}

std::vector<std::string_view> HelloWorld::GetPicFiles() {
	std::vector<std::string_view> picFiles;
	for (auto& f : files) {
		if (IsPic(f)) {
			picFiles.push_back(f);
		}
	}
	return picFiles;
}

std::string_view HelloWorld::GetSameName(std::vector<std::string_view> const& fs) {
	if (fs.empty()) return "";
	// 找出最短长度
	auto len = std::numeric_limits<size_t>::max();
	for (auto& f : fs) {
		if (len > f.size()) {
			len = f.size();
		}
	}
	// 一个个字符找不同
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

bool HelloWorld::Convertable() {
	auto fs = GetPicFiles();
	if (fs.empty()) return false;
	auto name = GetSameName(fs);
	if (name.empty()) return false;
	FillFps();
	FillRates();
	return !ratesString.empty();
}

void HelloWorld::FillRates() {
	// 过滤掉 ratesString 中的非 数字 逗号, 连续逗号，尾逗号
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

void HelloWorld::FillFps() {
	fps = std::strtol(fpsString.c_str(), nullptr, 10);
	if (fps <= 0 || fps > 120) {
		fps = 30;
	}
	fpsString = std::to_string(fps);
}

void HelloWorld::FillDefaultResPath() {
	resPath = std::filesystem::absolute(AppDelegate::resPath).string();
	if (resPath.back() != '\\' && resPath.back() != '/') {
		resPath += '\\';
	}
}

void HelloWorld::MoveUp() {
	if (selectedFile.empty()) {
		MoveTop();
		return;
	}
	auto iter = std::find(files.begin(), files.end(), selectedFile);
	if (iter != files.begin()) {
		selectedFile = *(--iter);
		DrawSelectedFile();
	}
}

void HelloWorld::MoveDown() {
	if (selectedFile.empty()) {
		MoveBottom();
		return;
	}
	auto iter = std::find(files.begin(), files.end(), selectedFile);
	if (++iter == files.end()) return;
	selectedFile = *iter;
	DrawSelectedFile();
}

void HelloWorld::MoveBottom() {
	if (files.empty()) return;
	selectedFile = files.back();
	DrawSelectedFile();
}

void HelloWorld::MoveTop() {
	if (files.empty()) return;
	selectedFile = files.front();
	DrawSelectedFile();
}

void HelloWorld::ZoomOut() {

}

void HelloWorld::ZoomIn() {

}


*/