#include "AppDelegate.h"
#include "xx_chrono.h"

AppDelegate::AppDelegate() {
	resRootPath = std::filesystem::current_path() / "res";
}
AppDelegate::~AppDelegate() {
}

int AppDelegate::Init() {
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
		monitor = glfwGetPrimaryMonitor();
	}
	window = glfwCreateWindow(width, height, "window's title", monitor, nullptr);
	if (!window) return __LINE__;
	glfwMakeContextCurrent(window);

	glfwSetMouseButtonCallback(window, [](auto w, auto b, auto a, auto m) { gAppDelegate->onGLFWMouseCallBack(b, a, m); });
	glfwSetCursorPosCallback(window, [](auto w, auto x, auto y) { gAppDelegate->onGLFWMouseMoveCallBack(x, y); });
	glfwSetScrollCallback(window, [](auto w, auto x, auto y) { gAppDelegate->onGLFWMouseScrollCallback(x, y); });
	glfwSetCharCallback(window, [](auto w, auto c) { gAppDelegate->onGLFWCharCallback(c); });
	glfwSetKeyCallback(window, [](auto w, auto k, auto s, auto a, auto m) { gAppDelegate->onGLFWKeyCallback(k, s, a, m); });
	glfwSetWindowPosCallback(window, [](auto w, auto x, auto y) { gAppDelegate->onGLFWWindowPosCallback(x, y); });
	glfwSetFramebufferSizeCallback(window, [](auto w_, auto w, auto h) { gAppDelegate->onGLFWframebuffersize(w, h); });
	glfwSetWindowSizeCallback(window, [](auto w_, auto w, auto h) { gAppDelegate->onGLFWWindowSizeFunCallback(w, h); });
	glfwSetWindowIconifyCallback(window, [](auto w, auto i) { gAppDelegate->onGLFWWindowIconifyCallback(i); });
	glfwSetWindowFocusCallback(window, [](auto w, auto f) { gAppDelegate->onGLFWWindowFocusCallback(f); });

	glfwGetWindowSize(window, &width, &height);
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

	_modelViewMatrixStack.push(Mat4_IDENTITY);
	std::stack<Mat4> projectionMatrixStack;
	projectionMatrixStack.push(Mat4_IDENTITY);
	_projectionMatrixStackList.push_back(projectionMatrixStack);
	_textureMatrixStack.push(Mat4_IDENTITY);

	// setProjection
	Mat4 orthoMatrix;
	Mat4::createOrthographicOffCenter(0, (float)width, 0, (float)height, -1024, 1024, &orthoMatrix);
	_projectionMatrixStackList[0].top() = orthoMatrix;
	_modelViewMatrixStack.top() = Mat4_IDENTITY;

	// setupBuffer:
	glGenVertexArrays(1, &_buffersVAO);
	glBindVertexArray(_buffersVAO);

	glGenBuffers(2, &_buffersVBO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);

	// vertices
	glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, vertices));

	// colors
	glEnableVertexAttribArray(VERTEX_ATTRIB_COLOR);
	glVertexAttribPointer(VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, colors));

	// tex coords
	glEnableVertexAttribArray(VERTEX_ATTRIB_TEX_COORD);
	glVertexAttribPointer(VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, texCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * INDEX_VBO_SIZE, _indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (auto r = glGetError()) {
		lastErrorNumber = r;
		lastErrorMessage = "gl error";
		return r;
	}

	return 0;
}

int AppDelegate::Run(int const& width, int const& height, bool const& fullScreen) {
	this->width = width;
	this->height = height;
	this->fullScreen = fullScreen;
	if (int r = Init()) return r;
	delta = xx::NowSteadyEpochSeconds();
	while (!glfwWindowShouldClose(window)) {
		delta = xx::NowSteadyEpochSeconds(delta);
		// todo: Draw
		glfwPollEvents();
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
