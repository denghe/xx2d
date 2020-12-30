#include "pch.h"
#include "AppDelegate.h"
#include "CCGeometry.h"
#include <shellapi.h>	// APPBARDATA

AppDelegate::AppDelegate() {
	m_resourceRootPath = std::filesystem::current_path() / "res";
}

const std::filesystem::path& AppDelegate::getResourceRootPath() {
	return m_resourceRootPath;
}

void AppDelegate::setTitle(std::string const& title) {
	m_title = title;
	if (m_hInstance) {
		// todo: set win title
	}
}

void AppDelegate::setFrameSize(float const& width, float const& height) {
	m_frameWidth = width;
	m_frameHeight = height;
	if (m_hInstance) {
		// todo: resize win
	}
}

void AppDelegate::setFrameZoomFactor(float const& factor) {
	m_frameZoomFactor = factor;
	if (m_hInstance) {
		// todo: resize win
	}
}

void AppDelegate::setAnimationInterval(double const& interval) {
	LARGE_INTEGER nFreq;
	QueryPerformanceFrequency(&nFreq);
	m_nAnimationInterval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);
}

static const WCHAR* kWindowClassName = L"CocoWin32";
static LRESULT CALLBACK _WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (gAppDelegate && gAppDelegate->m_hWnd == hWnd) {
		return gAppDelegate->WindowProc(uMsg, wParam, lParam);
	}
	else {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT AppDelegate::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	BOOL bProcessed = FALSE;
	switch (message) {
		//case WM_LBUTTONDOWN:
		//	if (m_pDelegate && MK_LBUTTON == wParam)
		//	{
		//		POINT point = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
		//		CCPoint pt(point.x, point.y);
		//		pt.x /= m_frameZoomFactor;
		//		pt.y /= m_frameZoomFactor;
		//		CCPoint tmp = ccp(pt.x, m_obScreenSize.height - pt.y);
		//		if (m_obViewPortRect.equals(CCRectZero) || m_obViewPortRect.containsPoint(tmp))
		//		{
		//			m_bCaptured = true;
		//			SetCapture(m_hWnd);
		//			int id = 0;
		//			handleTouchesBegin(1, &id, &pt.x, &pt.y);
		//		}
		//	}
		//	break;

		//case WM_MOUSEMOVE:
		//	if (MK_LBUTTON == wParam && m_bCaptured)
		//	{
		//		POINT point = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
		//		CCPoint pt(point.x, point.y);
		//		int id = 0;
		//		pt.x /= m_fFrameZoomFactor;
		//		pt.y /= m_fFrameZoomFactor;
		//		handleTouchesMove(1, &id, &pt.x, &pt.y);
		//	}
		//	break;

		//case WM_LBUTTONUP:
		//	if (m_bCaptured)
		//	{
		//		POINT point = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
		//		CCPoint pt(point.x, point.y);
		//		int id = 0;
		//		pt.x /= m_fFrameZoomFactor;
		//		pt.y /= m_fFrameZoomFactor;
		//		handleTouchesEnd(1, &id, &pt.x, &pt.y);

		//		ReleaseCapture();
		//		m_bCaptured = false;
		//	}
		//	break;
		//case WM_SIZE:
		//	switch (wParam)
		//	{
		//	case SIZE_RESTORED:
		//		CCApplication::sharedApplication()->applicationWillEnterForeground();
		//		break;
		//	case SIZE_MINIMIZED:
		//		CCApplication::sharedApplication()->applicationDidEnterBackground();
		//		break;
		//	}
		//	break;
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
		(int)(m_frameWidth * m_frameZoomFactor),        // Window Width
		(int)(m_frameHeight * m_frameZoomFactor),       // Window Height
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
	return 0;
}

void AppDelegate::destroyGL() {
}


//bool AppDelegate::applicationDidFinishLaunching() {
	//    // initialize director
	//    CCDirector* pDirector = CCDirector::sharedDirector();
	//    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
	//
	//    pDirector->setOpenGLView(pEGLView);
	//    CCSize frameSize = pEGLView->getFrameSize();
	//
	//    // Set the design resolution
	//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	//    pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionShowAll);
	//#else
	//    pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionNoBorder);
	//#endif
	//
	//
	//    vector<string> searchPath;
	//
	//    // In this demo, we select resource according to the frame's height.
	//    // If the resource size is different from design resolution size, you need to set contentScaleFactor.
	//    // We use the ratio of resource's height to the height of design resolution,
	//    // this can make sure that the resource's height could fit for the height of design resolution.
	//
	//    // if the frame's height is larger than the height of medium resource size, select large resource.
	//    if (frameSize.height > mediumResource.size.height)
	//    {
	//        searchPath.push_back(largeResource.directory);
	//
	//        pDirector->setContentScaleFactor(MIN(largeResource.size.height / designResolutionSize.height, largeResource.size.width / designResolutionSize.width));
	//    }
	//    // if the frame's height is larger than the height of small resource size, select medium resource.
	//    else if (frameSize.height > smallResource.size.height)
	//    {
	//        searchPath.push_back(mediumResource.directory);
	//
	//        pDirector->setContentScaleFactor(MIN(mediumResource.size.height / designResolutionSize.height, mediumResource.size.width / designResolutionSize.width));
	//    }
	//    // if the frame's height is smaller than the height of medium resource size, select small resource.
	//    else
	//    {
	//        searchPath.push_back(smallResource.directory);
	//
	//        pDirector->setContentScaleFactor(MIN(smallResource.size.height / designResolutionSize.height, smallResource.size.width / designResolutionSize.width));
	//    }
	//
	//
	//    // set searching path
	//    CCFileUtils::sharedFileUtils()->setSearchPaths(searchPath);
	//
	//    // turn on display FPS
	//    pDirector->setDisplayStats(true);
	//
	//    // set FPS. the default value is 1.0/60 if you don't call this
	//    pDirector->setAnimationInterval(1.0 / 60);
	//
	//    // create a scene. it's an autorelease object
	//    CCScene* pScene = HelloWorld::scene();
	//
	//    // run
	//    pDirector->runWithScene(pScene);

//	return true;
//}


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
