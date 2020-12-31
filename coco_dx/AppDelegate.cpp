#include "pch.h"
#include "AppDelegate.h"

AppDelegate::AppDelegate() {
	_resourceRootPath = std::filesystem::current_path() / "res";
	instance = this;
}

AppDelegate::~AppDelegate() {
	instance = nullptr;
}

const std::filesystem::path& AppDelegate::getResourceRootPath() {
	return _resourceRootPath;
}

void AppDelegate::setTitle(std::string const& title) {
	_title = title;
	if (_hWnd) {
		SetWindowTextA(_hWnd, _title.c_str());
	}
}

void AppDelegate::setFrameSize(CCSize const& siz) {
	assert(!_hWnd);
	_frameSize = siz;
	_screenSize = siz * _frameZoomFactor;
}

void AppDelegate::setFrameZoomFactor(float const& factor) {
	assert(!_hWnd);
	_frameZoomFactor = factor;
	_screenSize = _frameSize * factor;
}

void AppDelegate::setAnimationInterval(double const& interval) {
	_animationInterval = interval;
}

void AppDelegate::setDesignResolutionSize(CCSize const& siz, ResolutionPolicy const& policy) {
	_designResolutionSize = siz;

	_scaleX = (float)_screenSize.width / _designResolutionSize.width;
	_scaleY = (float)_screenSize.height / _designResolutionSize.height;

	switch (policy) {
	case ResolutionPolicy::NoBorder:
		_scaleX = _scaleY = std::max(_scaleX, _scaleY);
		break;
	case ResolutionPolicy::ShowAll:
		_scaleX = _scaleY = std::min(_scaleX, _scaleY);
		break;
	case ResolutionPolicy::FixedHeight:
		_scaleX = _scaleY;
		_designResolutionSize.width = ceilf(_screenSize.width / _scaleX);
		break;
	case ResolutionPolicy::FixedWidth:
		_scaleY = _scaleX;
		_designResolutionSize.height = ceilf(_screenSize.height / _scaleY);
		break;
	}

	// calculate the rect of viewport    
	float viewPortW = _designResolutionSize.width * _scaleX;
	float viewPortH = _designResolutionSize.height * _scaleY;

	_viewPortRect.setRect((_screenSize.width - viewPortW) / 2, (_screenSize.height - viewPortH) / 2, viewPortW, viewPortH);

	_resolutionPolicy = policy;

	// reset director's member variables to fit visible rect
	_winSizeInPoints = _designResolutionSize;
}

int AppDelegate::InitWindow(HINSTANCE hInstance, int nCmdShow) {
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)->LRESULT { return instance->WndProc(hWnd, message, wParam, lParam); };
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, L"IDI_ICON");
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"coco_dx";
	wcex.hIconSm = LoadIcon(wcex.hInstance, L"IDI_ICON");
	if (!RegisterClassEx(&wcex)) return GetLastError();

	_hInst = hInstance;
	RECT rc{ 0, 0, (LONG)_screenSize.width, (LONG)_screenSize.height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	_hWnd = CreateWindowEx(0, L"coco_dx", L""
		, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT
		, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	if (!_hWnd) return GetLastError();

	SetWindowTextA(_hWnd, _title.c_str());

	ShowWindow(_hWnd, nCmdShow);
	return 0;
}

LRESULT AppDelegate::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps;
		auto&& hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


int AppDelegate::run() {
	_beginTime = _lastTime = xx::NowEpochSeconds();
	_fpsTimer = _lastTime + 1;

	MSG msg = {};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			++_counter;
			auto delta = xx::NowEpochSeconds(_lastTime);
			if (_fpsTimer < _lastTime) {
				_fpsTimer = _lastTime + 1;
				SetWindowTextA(_hWnd, std::to_string((double)_counter / (_lastTime - _beginTime)).c_str());
			}
			//Render();
		}
	}
	//CleanupDevice();
	return (int)msg.wParam;
}
