#pragma once
#include "pch.h"

struct AppDelegate {
    AppDelegate();
    ~AppDelegate();

    std::filesystem::path const& getResourceRootPath();
    void setTitle(std::string const& title);
    void setFrameSize(CCSize const& siz);
    void setFrameZoomFactor(float const& factor);
    void setAnimationInterval(double const& interval);
    void setDesignResolutionSize(CCSize const& siz, ResolutionPolicy const& policy);

    //void setGLDefaultValues();
    //void setAlphaBlending(bool const& bOn);
    //void setDepthTest(bool const& bOn);

    int InitWindow(HINSTANCE hInstance, int nCmdShow);
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    int run();

    std::function<bool()> onDidFinishLaunching;
    std::function<void()> onMainLoop;
    std::function<void()> onDidEnterBackground;
    std::function<void()> onWillEnterForeground;


    std::filesystem::path           _resourceRootPath;
    std::string                     _title;
    bool                            _fullScreen = false;
    CCSize                          _frameSize;
    float                           _frameZoomFactor = 1.f;
    CCSize                          _screenSize;
    CCSize                          _winSizeInPoints;
    CCSize                          _designResolutionSize;
    CCRect                          _viewPortRect;
    ResolutionPolicy                _resolutionPolicy;
    float                           _scaleX = 1.f;
    float                           _scaleY = 1.f;
    double                          _animationInterval = 1.0 / 60.0;

    double                          _beginTime = 0;
    double                          _lastTime = 0;
    double                          _fpsTimer = 0;
    uint64_t                        _counter = 0;

    HINSTANCE					    _hInst = nullptr;
    HWND						    _hWnd = nullptr;

    bool                            _captured = false;  // mouse

    int                             _lastErrorNumber = 0;
    std::string                     _lastErrorMessage;

    inline static AppDelegate*      instance = nullptr;
};
