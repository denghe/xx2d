#pragma once
#include "pch.h"

struct AppDelegate {
    AppDelegate();
    ~AppDelegate() = default;
    const std::filesystem::path& getResourceRootPath();

    void setTitle(std::string const& title);
    void setFrameSize(float const& width, float const& height);
    void setFrameZoomFactor(float const& factor);
    void setAnimationInterval(double const& interval);

    int initWindow();
    int initGL();
    void destroyGL();
    int run();

    std::function<bool()> onDidFinishLaunching;
    std::function<void()> onMainLoop;
    std::function<void()> onDidEnterBackground;
    std::function<void()> onWillEnterForeground;

    typedef LRESULT(*CUSTOM_WND_PROC)(UINT message, WPARAM wParam, LPARAM lParam, BOOL* pProcessed);
    void setWndProc(CUSTOM_WND_PROC proc);

    typedef void (*LPFN_ACCELEROMETER_KEYHOOK)(UINT message, WPARAM wParam, LPARAM lParam);
    void setAccelerometerKeyHook(LPFN_ACCELEROMETER_KEYHOOK lpfnAccelerometerKeyHook);

    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    HINSTANCE                       m_hInstance = nullptr;
    HACCEL                          m_hAccelTable = nullptr;
    LARGE_INTEGER                   m_nAnimationInterval;
    std::filesystem::path           m_resourceRootPath;
    std::string                     m_title;
    float                           m_frameWidth = 0.f;
    float                           m_frameHeight = 0.f;

    bool                            m_bCaptured = false;
    HWND                            m_hWnd = nullptr;
    HDC                             m_hDC = nullptr;
    HGLRC                           m_hRC = nullptr;
    LPFN_ACCELEROMETER_KEYHOOK      m_lpfnAccelerometerKeyHook = nullptr;
    bool                            m_bSupportTouch = false;

    LPCWSTR                         m_menu = nullptr;
    CUSTOM_WND_PROC                 m_wndproc = nullptr;
    float                           m_frameZoomFactor = 1.f;
};

// 全局单例. main 里 emplace 创建
inline static xx::Shared<AppDelegate> gAppDelegate;
