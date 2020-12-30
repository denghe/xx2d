#include "pch.h"
#include "AppDelegate.h"

AppDelegate::AppDelegate() {
	m_resourceRootPath = std::filesystem::current_path() / "res";
}

const std::filesystem::path& AppDelegate::getResourceRootPath() {
	return m_resourceRootPath;
}

void AppDelegate::setTitle(std::string const& title) {
	m_title = title;
	if (m_hWnd) {
		WCHAR wszBuf[MAX_PATH] = { 0 };
		MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, wszBuf, sizeof(wszBuf));
		SetWindowText(m_hWnd, wszBuf);
	}
}

void AppDelegate::setFrameSize(CCSize const& siz) {
	assert(!m_hWnd);
	m_frameSize = siz;
	m_obScreenSize = siz * m_frameZoomFactor;
}

void AppDelegate::setFrameZoomFactor(float const& factor) {
	assert(!m_hWnd);
	m_frameZoomFactor = factor;
	m_obScreenSize = m_frameSize * factor;
}

void AppDelegate::setAnimationInterval(double const& interval) {
	LARGE_INTEGER nFreq;
	QueryPerformanceFrequency(&nFreq);
	m_nAnimationInterval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);
}

void AppDelegate::setDesignResolutionSize(CCSize const& siz, ResolutionPolicy const& policy) {
	assert(!m_hWnd);
	m_obDesignResolutionSize = siz;

	m_fScaleX = (float)m_obScreenSize.width / m_obDesignResolutionSize.width;
	m_fScaleY = (float)m_obScreenSize.height / m_obDesignResolutionSize.height;

	switch (policy) {
	case ResolutionPolicy::NoBorder:
		m_fScaleX = m_fScaleY = std::max(m_fScaleX, m_fScaleY);
		break;
	case ResolutionPolicy::ShowAll:
		m_fScaleX = m_fScaleY = std::min(m_fScaleX, m_fScaleY);
		break;
	case ResolutionPolicy::FixedHeight:
		m_fScaleX = m_fScaleY;
		m_obDesignResolutionSize.width = ceilf(m_obScreenSize.width / m_fScaleX);
		break;
	case ResolutionPolicy::FixedWidth:
		m_fScaleY = m_fScaleX;
		m_obDesignResolutionSize.height = ceilf(m_obScreenSize.height / m_fScaleY);
		break;
	}

	// calculate the rect of viewport    
	float viewPortW = m_obDesignResolutionSize.width * m_fScaleX;
	float viewPortH = m_obDesignResolutionSize.height * m_fScaleY;

	m_obViewPortRect.setRect((m_obScreenSize.width - viewPortW) / 2, (m_obScreenSize.height - viewPortH) / 2, viewPortW, viewPortH);

	m_eResolutionPolicy = policy;

	// reset director's member variables to fit visible rect
	m_obWinSizeInPoints = m_obDesignResolutionSize;
}

static const WCHAR* kWindowClassName = L"CocoWin32";
static LRESULT CALLBACK _WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	std::cout << gAppDelegate->m_hWnd << "---" << hWnd << std::endl;
	if (gAppDelegate && gAppDelegate->m_hWnd == hWnd) {
		return gAppDelegate->WindowProc(uMsg, wParam, lParam);
	}
	else {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT AppDelegate::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	std::cout << "WindowProc" << std::endl;
	BOOL bProcessed = FALSE;
	switch (message) {
	case WM_LBUTTONDOWN:
		std::cout << "WM_LBUTTONDOWN" << std::endl;
		if (/*m_pDelegate && */MK_LBUTTON == wParam) {
			POINT point = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
			auto pt = CCPointMake(point.x, point.y);
			pt.x /= m_frameZoomFactor;
			pt.y /= m_frameZoomFactor;
			CCPoint tmp(pt.x, m_obScreenSize.height - pt.y);
			if (m_obViewPortRect.equals(CCRectZero) || m_obViewPortRect.containsPoint(tmp)) {
				m_bCaptured = true;
				SetCapture(m_hWnd);
				int id = 0;
				//handleTouchesBegin(1, &id, &pt.x, &pt.y);
				std::cout << "handleTouchesBegin " << pt.x << " " << pt.y << std::endl;
			}
		}
		break;

	case WM_MOUSEMOVE:
		std::cout << "WM_MOUSEMOVE" << std::endl;
		if (MK_LBUTTON == wParam && m_bCaptured)
		{
			POINT point = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
			auto pt = CCPointMake(point.x, point.y);
			int id = 0;
			pt.x /= m_frameZoomFactor;
			pt.y /= m_frameZoomFactor;
			//handleTouchesMove(1, &id, &pt.x, &pt.y);
			std::cout << "handleTouchesMove " << pt.x << " " << pt.y << std::endl;
		}
		break;

	case WM_LBUTTONUP:
		std::cout << "WM_LBUTTONUP" << std::endl;
		if (m_bCaptured) {
			POINT point = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
			auto pt = CCPointMake(point.x, point.y);
			int id = 0;
			pt.x /= m_frameZoomFactor;
			pt.y /= m_frameZoomFactor;
			//handleTouchesEnd(1, &id, &pt.x, &pt.y);
			std::cout << "handleTouchesEnd " << pt.x << " " << pt.y << std::endl;

			ReleaseCapture();
			m_bCaptured = false;
		}
		break;

	case WM_SIZE:
		switch (wParam)
		{
		case SIZE_RESTORED:
			if (onDidEnterBackground) {
				onDidEnterBackground();
			}
			break;
		case SIZE_MINIMIZED:
			if (onWillEnterForeground) {
				onWillEnterForeground();
			}
			break;
		}
		break;

		//case WM_KEYDOWN:
		//	if (wParam == VK_F1 || wParam == VK_F2)
		//	{
		//		CCDirector* pDirector = CCDirector::sharedDirector();
		//		if (GetKeyState(VK_LSHIFT) < 0 || GetKeyState(VK_RSHIFT) < 0 || GetKeyState(VK_SHIFT) < 0)
		//			pDirector->getKeypadDispatcher()->dispatchKeypadMSG(wParam == VK_F1 ? kTypeBackClicked : kTypeMenuClicked);
		//	}
		//	else if (wParam == VK_ESCAPE)
		//	{
		//		CCDirector::sharedDirector()->getKeypadDispatcher()->dispatchKeypadMSG(kTypeBackClicked);
		//	}

		//	if (m_lpfnAccelerometerKeyHook != nullptr)
		//	{
		//		(*m_lpfnAccelerometerKeyHook)(message, wParam, lParam);
		//	}
		//	break;
		//case WM_KEYUP:
		//	if (m_lpfnAccelerometerKeyHook != nullptr)
		//	{
		//		(*m_lpfnAccelerometerKeyHook)(message, wParam, lParam);
		//	}
		//	break;

		//case WM_CHAR:
		//{
		//	if (wParam < 0x20)
		//	{
		//		if (VK_BACK == wParam)
		//		{
		//			CCIMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
		//		}
		//		else if (VK_RETURN == wParam)
		//		{
		//			CCIMEDispatcher::sharedDispatcher()->dispatchInsertText("\n", 1);
		//		}
		//		else if (VK_TAB == wParam)
		//		{
		//			// tab input
		//		}
		//		else if (VK_ESCAPE == wParam)
		//		{
		//			// ESC input
		//			//CCDirector::sharedDirector()->end();
		//		}
		//	}
		//	else if (wParam < 128)
		//	{
		//		// ascii char
		//		CCIMEDispatcher::sharedDispatcher()->dispatchInsertText((const char*)&wParam, 1);
		//	}
		//	else
		//	{
		//		char szUtf8[8] = { 0 };
		//		int nLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)&wParam, 1, szUtf8, sizeof(szUtf8), nullptr, nullptr);
		//		CCIMEDispatcher::sharedDispatcher()->dispatchInsertText(szUtf8, nLen);
		//	}
		//	if (m_lpfnAccelerometerKeyHook != nullptr)
		//	{
		//		(*m_lpfnAccelerometerKeyHook)(message, wParam, lParam);
		//	}
		//}
		//break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(m_hWnd, &ps);
		EndPaint(m_hWnd, &ps);
		break;

	case WM_CLOSE:
		//CCDirector::sharedDirector()->end();
		break;

	case WM_DESTROY:
		destroyGL();
		PostQuitMessage(0);
		break;

	default:
		if (m_wndproc)
		{

			m_wndproc(message, wParam, lParam, &bProcessed);
			if (bProcessed) break;
		}
		return DefWindowProc(m_hWnd, message, wParam, lParam);
	}

	if (m_wndproc && !bProcessed)
	{
		m_wndproc(message, wParam, lParam, &bProcessed);
	}
	return 0;
}

int AppDelegate::initWindow() {
	if (m_hWnd) return __LINE__;

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	WNDCLASS  wc;        // Windows Class Structure

	// Redraw On Size, And Own DC For Window.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = _WindowProc;						// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);		// Load The Arrow Pointer
	wc.hbrBackground = nullptr;                         // No Background Required For GL
	wc.lpszMenuName = m_menu;							//
	wc.lpszClassName = kWindowClassName;				// Set The Class Name

	if (!RegisterClass(&wc)) return GetLastError();

	// center window position
	RECT rcDesktop;
	GetWindowRect(GetDesktopWindow(), &rcDesktop);

	WCHAR wszBuf[50] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, m_title.c_str(), -1, wszBuf, sizeof(wszBuf));

	// create window
	m_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,				// Extended Style For The Window
		kWindowClassName,                               // Class Name
		wszBuf,                                         // Window Title
		WS_CAPTION | WS_POPUPWINDOW | WS_MINIMIZEBOX,   // Defined Window Style
		0, 0,                                           // Window Position
		//TODO: Initializing width with a large value to avoid getting a wrong client area by 'GetClientRect' function.
		(int)m_obScreenSize.width,						// Window Width
		(int)m_obScreenSize.height,						// Window Height
		nullptr,                                        // No Parent Window
		nullptr,                                        // No Menu
		hInstance,                                      // Instance
		nullptr);

	if (!m_hWnd) return GetLastError();


	RECT rcWindow;

	// substract the task bar
	if (HWND hTaskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL)) {
		APPBARDATA abd;
		abd.cbSize = sizeof(APPBARDATA);
		abd.hWnd = hTaskBar;

		SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
		SubtractRect(&rcDesktop, &rcDesktop, &abd.rc);
	}
	GetWindowRect(m_hWnd, &rcWindow);
	m_obScreenSize.setSize((float)(rcWindow.right - rcWindow.left), (float)(rcWindow.bottom - rcWindow.top));


	int offsetX = rcDesktop.left + (rcDesktop.right - rcDesktop.left - (rcWindow.right - rcWindow.left)) / 2;
	offsetX = (offsetX > 0) ? offsetX : rcDesktop.left;
	int offsetY = rcDesktop.top + (rcDesktop.bottom - rcDesktop.top - (rcWindow.bottom - rcWindow.top)) / 2;
	offsetY = (offsetY > 0) ? offsetY : rcDesktop.top;

	SetWindowPos(m_hWnd, 0, offsetX, offsetY, 0, 0, SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);


	if (int r = initGL()) {
		destroyGL();
		return r;
	}

	return 0;
}

int AppDelegate::initGL() {
	// todo
	//setGLDefaultValues();
	return 0;
}

void AppDelegate::destroyGL() {
}

void AppDelegate::setGLDefaultValues() {
	assert(m_hWnd);

	setAlphaBlending(true);
	// XXX: Fix me, should enable/disable depth test according the depth format as cocos2d-iphone did
	// [self setDepthTest: view_.depthFormat];
	setDepthTest(false);
	//setProjection(m_eProjection);

	// set other opengl default values
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void AppDelegate::setAlphaBlending(bool const& bOn) {
	if (bOn) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glDisable(GL_BLEND);
	}
	CHECK_GL_ERROR_DEBUG();
}

void AppDelegate::setDepthTest(bool const& bOn) {
	if (bOn) {
		glClearDepthf(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	CHECK_GL_ERROR_DEBUG();
}


int AppDelegate::run() {
	// Main message loop:
	MSG msg;
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nLast;
	LARGE_INTEGER nNow;

	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nLast);

	// Initialize instance and cocos2d.
	if (onDidFinishLaunching) {
		if (!onDidFinishLaunching()) return 0;
	}

	//pMainWnd->centerWindow();
	ShowWindow(m_hWnd, SW_SHOW);

	while (1) {
		if (!PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			// Get current time tick.
			QueryPerformanceCounter(&nNow);

			// If it's the time to draw next frame, draw it, else sleep a while.
			if (nNow.QuadPart - nLast.QuadPart > m_nAnimationInterval.QuadPart) {
				nLast.QuadPart = nNow.QuadPart;
				if (onMainLoop) {
					onMainLoop();
				}
			}
			else {
				Sleep(0);
			}
			continue;
		}

		// Quit message loop.
		if (WM_QUIT == msg.message)	break;

		// Deal with windows message.
		if (!m_hAccelTable || !TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
