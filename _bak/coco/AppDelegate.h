#pragma once
#include "pch.h"
#include <filesystem>

struct AppDelegate {
    AppDelegate();
    ~AppDelegate();

    const std::filesystem::path& getResourceRootPath();
    void setTitle(std::string const& title);
    void setFrameSize(CCSize const& siz);
    void setFrameZoomFactor(float const& factor);
    void setAnimationInterval(double const& interval);
    void setDesignResolutionSize(CCSize const& siz, ResolutionPolicy const& policy);

    //void setGLDefaultValues();
    //void setAlphaBlending(bool const& bOn);
    //void setDepthTest(bool const& bOn);
    int run();

    std::function<bool()> onDidFinishLaunching;
    std::function<void()> onMainLoop;
    std::function<void()> onDidEnterBackground;
    std::function<void()> onWillEnterForeground;

protected:
    void onGLFWError(int errorID, const char* errorDesc);
    void onGLFWMouseCallBack(int button, int action, int modify);
    void onGLFWMouseMoveCallBack(double x, double y);
    void onGLFWMouseScrollCallback(double x, double y);
    void onGLFWKeyCallback(int key, int scancode, int action, int mods);
    void onGLFWCharCallback(unsigned int character);
    void onGLFWWindowPosCallback(int x, int y);
    void onGLFWframebuffersize(int w, int h);
    void onGLFWWindowSizeFunCallback(int width, int height);
    void onGLFWWindowIconifyCallback(int iconified);
    void onGLFWWindowFocusCallback(int focused);

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
    uint64_t                        _numFrames = 0;

    GLFWmonitor*                    _monitor = nullptr;
    GLFWwindow*                     _wnd = nullptr;

    bool                            _captured = false;  // mouse

    //static const int VBO_SIZE = 65536;
    //static const int INDEX_VBO_SIZE = VBO_SIZE * 6 / 4;
    //static const int BATCH_TRIAGCOMMAND_RESERVED_SIZE = 64;
    //static const int MATERIAL_ID_DO_NOT_BATCH = 0;

    //V3F_C4B_T2F _verts[VBO_SIZE];
    //GLushort _indices[INDEX_VBO_SIZE];
    //GLuint _buffersVAO;
    //GLuint _buffersVBO[2]; //0: vertex  1: indices

public:
    int                             _lastErrorNumber = 0;
    std::string                     _lastErrorMessage;

    inline static AppDelegate*      instance = nullptr;
};
