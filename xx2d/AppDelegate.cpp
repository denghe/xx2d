#include "AppDelegate.h"
#include "xx_chrono.h"

AppDelegate::AppDelegate() {
	resRootPath = std::filesystem::current_path() / "res";
}
AppDelegate::~AppDelegate() {
	// todo: release glxxx ?
}

int AppDelegate::Run(int const& width_, int const& height_, bool const& fullScreen_) {
	width = width_;
	height = height_;
	fullScreen = fullScreen_;

	glfwSetErrorCallback([](auto n, auto m) { gAppDelegate->onGLFWError(n, m); });
	if (glfwInit() == GLFW_FALSE) return __LINE__;

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_SAMPLES, 0);
	if (fullScreen) {
		_monitor = glfwGetPrimaryMonitor();
	}
	_window = glfwCreateWindow(width, height, "window's title", _monitor, nullptr);
	if (!_window) return __LINE__;
	glfwMakeContextCurrent(_window);

	glfwSetMouseButtonCallback(_window, [](auto w, auto b, auto a, auto m) { gAppDelegate->onGLFWMouseCallBack(b, a, m); });
	glfwSetCursorPosCallback(_window, [](auto w, auto x, auto y) { gAppDelegate->onGLFWMouseMoveCallBack(x, y); });
	glfwSetScrollCallback(_window, [](auto w, auto x, auto y) { gAppDelegate->onGLFWMouseScrollCallback(x, y); });
	glfwSetCharCallback(_window, [](auto w, auto c) { gAppDelegate->onGLFWCharCallback(c); });
	glfwSetKeyCallback(_window, [](auto w, auto k, auto s, auto a, auto m) { gAppDelegate->onGLFWKeyCallback(k, s, a, m); });
	glfwSetWindowPosCallback(_window, [](auto w, auto x, auto y) { gAppDelegate->onGLFWWindowPosCallback(x, y); });
	glfwSetFramebufferSizeCallback(_window, [](auto w_, auto w, auto h) { gAppDelegate->onGLFWframebuffersize(w, h); });
	glfwSetWindowSizeCallback(_window, [](auto w_, auto w, auto h) { gAppDelegate->onGLFWWindowSizeFunCallback(w, h); });
	glfwSetWindowIconifyCallback(_window, [](auto w, auto i) { gAppDelegate->onGLFWWindowIconifyCallback(i); });
	glfwSetWindowFocusCallback(_window, [](auto w, auto f) { gAppDelegate->onGLFWWindowFocusCallback(f); });

	glfwGetWindowSize(_window, &width, &height);
	//glfwSwapInterval(1);


	auto r = glewInit();
	if (GLEW_OK != r) {
		lastErrorNumber = (int)r;
		lastErrorMessage = (char*)glewGetErrorString(r);
		return lastErrorNumber;
	}
	if (glGenFramebuffers == nullptr) {
		lastErrorNumber = __LINE__;
		lastErrorMessage = "does not support glGenFramebuffers";
		return lastErrorNumber;
	}

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	glGenVertexArrays(1, &_buffersVAO);
	glBindVertexArray(_buffersVAO);

	glGenBuffers(2, &_buffersVBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);

	glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, vertices));
	glEnableVertexAttribArray(VERTEX_ATTRIB_COLOR);
	glVertexAttribPointer(VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, colors));
	glEnableVertexAttribArray(VERTEX_ATTRIB_TEX_COORD);
	glVertexAttribPointer(VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, texCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * INDEX_VBO_SIZE, _indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (auto r = glGetError()) {
		lastErrorNumber = r;
		lastErrorMessage = "gl error";
		return r;
	}


	_modelViewMatrixStack.push(Mat4_IDENTITY);
	_projectionMatrixStackList.emplace_back().emplace(Mat4_IDENTITY);
	_textureMatrixStack.push(Mat4_IDENTITY);
	Mat4::createOrthographicOffCenter(0, (float)width, 0, (float)height, -1024, 1024, &_projectionMatrixStackList[0].top());


	lastTime = xx::NowSteadyEpochSeconds();
	beginTime = lastTime - 0.0000000001;
	while (!glfwWindowShouldClose(_window)) {
		delta = xx::NowSteadyEpochSeconds(lastTime);
		++numFrames;
		glfwSetWindowTitle(_window, std::to_string((double)numFrames / (lastTime - beginTime)).c_str());

		glfwPollEvents();
		// todo: event handles

		// todo: frame logic

		glDepthMask(true);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(false);

		_modelViewMatrixStack.push(_modelViewMatrixStack.top());

		_drawnBatches = _drawnVertices = 0;


		// todo: draw? render?

		glfwSwapBuffers(_window);
	}
	//...
	return 0;
}


void AppDelegate::onGLFWError(int errorID, const char* errorDesc) {
	lastErrorNumber = errorID;
	lastErrorMessage = errorDesc;
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
