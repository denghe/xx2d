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
	if (_wnd) {
		glfwSetWindowTitle(_wnd, title.c_str());
	}
}

void AppDelegate::setFrameSize(CCSize const& siz) {
	assert(!_wnd);
	_frameSize = siz;
	_screenSize = siz * _frameZoomFactor;
}

void AppDelegate::setFrameZoomFactor(float const& factor) {
	assert(!_wnd);
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

int AppDelegate::run() {
	/***********************************************************************************************************************/
	glfwSetErrorCallback([](auto n, auto m) { instance->onGLFWError(n, m); });
	if (glfwInit() == GLFW_FALSE) return __LINE__;

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_SAMPLES, 0);
	if (_fullScreen) {
		_monitor = glfwGetPrimaryMonitor();
	}
	_wnd = glfwCreateWindow((int)_screenSize.width, (int)_screenSize.height, "window's title", _monitor, nullptr);
	if (!_wnd) return __LINE__;
	glfwMakeContextCurrent(_wnd);
	int w, h;
	glfwGetWindowSize(_wnd, &w, &h);
	_screenSize.setSize((float)w, (float)h);
	setDesignResolutionSize(_designResolutionSize, _resolutionPolicy);

	/***********************************************************************************************************************/
	glfwSetMouseButtonCallback(_wnd, [](auto w, auto b, auto a, auto m) { instance->onGLFWMouseCallBack(b, a, m); });
	glfwSetCursorPosCallback(_wnd, [](auto w, auto x, auto y) { instance->onGLFWMouseMoveCallBack(x, y); });
	glfwSetScrollCallback(_wnd, [](auto w, auto x, auto y) { instance->onGLFWMouseScrollCallback(x, y); });
	glfwSetCharCallback(_wnd, [](auto w, auto c) { instance->onGLFWCharCallback(c); });
	glfwSetKeyCallback(_wnd, [](auto w, auto k, auto s, auto a, auto m) { instance->onGLFWKeyCallback(k, s, a, m); });
	glfwSetWindowPosCallback(_wnd, [](auto w, auto x, auto y) { instance->onGLFWWindowPosCallback(x, y); });
	glfwSetFramebufferSizeCallback(_wnd, [](auto w_, auto w, auto h) { instance->onGLFWframebuffersize(w, h); });
	glfwSetWindowSizeCallback(_wnd, [](auto w_, auto w, auto h) { instance->onGLFWWindowSizeFunCallback(w, h); });
	glfwSetWindowIconifyCallback(_wnd, [](auto w, auto i) { instance->onGLFWWindowIconifyCallback(i); });
	glfwSetWindowFocusCallback(_wnd, [](auto w, auto f) { instance->onGLFWWindowFocusCallback(f); });

	/***********************************************************************************************************************/
	auto r = glewInit();
	if (GLEW_OK != r) {
		_lastErrorMessage = (char*)glewGetErrorString(r);
		return r;
	}

	if (glGenFramebuffers == nullptr) {
		_lastErrorMessage = "does not support glGenFramebuffers";
		return __LINE__;
	}

	if (!GLEW_ARB_vertex_shader || !GLEW_ARB_fragment_shader) {
		_lastErrorMessage = "does not support GLSL";
		return __LINE__;
	}

	if (!glewIsSupported("GL_VERSION_2_0")) {
		_lastErrorMessage = "OpenGL 2.0 not supported";
		return __LINE__;
	}

	if (!glGenFramebuffers) {
		_lastErrorMessage = "OpenGL: glGenFramebuffers is NULL";
		return __LINE__;
	}

	/***********************************************************************************************************************/
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);

	if (auto r = glGetError()) {
		_lastErrorMessage = "gl error";
		return r;
	}

	/***********************************************************************************************************************/
	//glGenVertexArrays(1, &_buffersVAO);
	//glBindVertexArray(_buffersVAO);

	//glGenBuffers(2, &_buffersVBO[0]);
	//glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);

	//glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION);
	//glVertexAttribPointer(VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, vertices));
	//glEnableVertexAttribArray(VERTEX_ATTRIB_COLOR);
	//glVertexAttribPointer(VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, colors));
	//glEnableVertexAttribArray(VERTEX_ATTRIB_TEX_COORD);
	//glVertexAttribPointer(VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, texCoords));

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * INDEX_VBO_SIZE, _indices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	/***********************************************************************************************************************/
	_lastTime = xx::NowSteadyEpochSeconds();
	_beginTime = _lastTime - 0.0000000001;
	while (!glfwWindowShouldClose(_wnd)) {
		_delta = xx::NowSteadyEpochSeconds(_lastTime);
		++_numFrames;
		// draw title
		glfwSetWindowTitle(_wnd, std::to_string((double)_numFrames / (_lastTime - _beginTime)).c_str());

		glfwPollEvents();
		// todo: event handles
		// todo: frame logic

		//glDepthMask(true);
		glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT*/);
		//glDepthMask(false);

		//_modelViewMatrixStack.push(_modelViewMatrixStack.top());

		//_drawnBatches = _drawnVertices = 0;


		// todo: draw? render?

		glfwSwapBuffers(_wnd);
	}
	//...
	return 0;
}

void AppDelegate::onGLFWError(int errorID, const char* errorDesc) {
	_lastErrorNumber = errorID;
	_lastErrorMessage = errorDesc;
}
void AppDelegate::onGLFWMouseCallBack(int button, int action, int modify) {
	std::cout << button << " " << action << " " << modify << std::endl;
}
void AppDelegate::onGLFWMouseMoveCallBack(double x, double y) {
	std::cout << x << " " << y << std::endl;
}
void AppDelegate::onGLFWMouseScrollCallback(double x, double y) {
	std::cout << x << " " << y << std::endl;
}
void AppDelegate::onGLFWKeyCallback(int key, int scancode, int action, int mods) {

}
void AppDelegate::onGLFWCharCallback(unsigned int character) {

}
void AppDelegate::onGLFWWindowPosCallback(int x, int y) {

}
void AppDelegate::onGLFWframebuffersize(int w, int h) {

}
void AppDelegate::onGLFWWindowSizeFunCallback(int width, int height) {

}
void AppDelegate::onGLFWWindowIconifyCallback(int iconified) {

}
void AppDelegate::onGLFWWindowFocusCallback(int focused) {

}
