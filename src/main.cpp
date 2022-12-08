#include"pch.h"
#include "logic.h"
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>
#include <glfw/glfw3ext.h>

xx::Shared<Logic> logic = xx::Make<Logic>();
GLFWwindow* wnd = nullptr;
inline int width = 0;
inline int height = 0;

int main() {
#ifdef OPEN_MP_NUM_THREADS
	omp_set_num_threads(OPEN_MP_NUM_THREADS);
	omp_set_dynamic(0);
#endif

	glfwSetErrorCallback([](int error, const char* description) {
		throw new std::exception(description, error);
		});

#if defined(_WIN32)
	if (!glfwxInit()) return -1;
	auto sg_glfw = xx::MakeSimpleScopeGuard([] { glfwxTerminate(); });
#else
	if (!glfwInit()) return -1;
	auto sg_glfw = xx::MakeSimpleScopeGuard([] { glfwTerminate(); });
#endif

	//glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	//glfwWindowHint(GLFW_RED_BITS, 8);
	//glfwWindowHint(GLFW_GREEN_BITS, 8);
	//glfwWindowHint(GLFW_BLUE_BITS, 8);
	//glfwWindowHint(GLFW_ALPHA_BITS, 8);
	//glfwWindowHint(GLFW_DEPTH_BITS, 24);
	//glfwWindowHint(GLFW_STENCIL_BITS, 8);
	//glfwWindowHint(GLFW_SAMPLES, 0);
	//glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	//glfwWindowHint(GLFW_DECORATED, GL_TRUE);
	//glfwxSetParent((HWND)0);

	wnd = glfwCreateWindow(logic->w, logic->h, "xx2dtest1", nullptr, nullptr);
	if (!wnd) return -2;
	auto sg_wnd = xx::MakeSimpleScopeGuard([&] { glfwDestroyWindow(wnd); });
	glfwMakeContextCurrent(wnd);

	//int realW = 0, realH = 0;
	//glfwGetWindowSize(_mainWindow, &realW, &realH);
	//if (realW != neededWidth) {
	//	rect.size.width = realW / _frameZoomFactor;
	//}
	//if (realH != neededHeight) {
	//	rect.size.height = realH / _frameZoomFactor;
	//}

	//glfwSetMouseButtonCallback(wnd, GLFWEventHandler::onGLFWMouseCallBack);
	//glfwSetCursorPosCallback(wnd, GLFWEventHandler::onGLFWMouseMoveCallBack);
	//glfwSetScrollCallback(wnd, GLFWEventHandler::onGLFWMouseScrollCallback);
	//glfwSetCharCallback(wnd, GLFWEventHandler::onGLFWCharCallback);
	////glfwSetKeyCallback(wnd, GLFWEventHandler::onGLFWKeyCallback);
	//glfwSetWindowPosCallback(wnd, GLFWEventHandler::onGLFWWindowPosCallback);
	////glfwSetFramebufferSizeCallback(wnd, GLFWEventHandler::onGLFWframebuffersize);
	//glfwSetWindowSizeCallback(wnd, GLFWEventHandler::onGLFWWindowSizeFunCallback);
	//glfwSetWindowIconifyCallback(wnd, GLFWEventHandler::onGLFWWindowIconifyCallback);
	//glfwSetWindowFocusCallback(wnd, GLFWEventHandler::onGLFWWindowFocusCallback);

	glfwSetKeyCallback(wnd, [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
		assert(wnd == ::wnd);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(wnd, GLFW_TRUE);
		}
		// todo: set key to logic
	});

	// todo: send mouse event to logic

	glfwSetFramebufferSizeCallback(wnd, [](GLFWwindow* wnd, int width, int height) {
		assert(wnd == ::wnd);
		::logic->w = width;
		::logic->h = height;
		// todo: add event to logic
	});
	glfwGetFramebufferSize(wnd, &width, &height);


	if (!gladLoadGL(glfwGetProcAddress)) return -3;
	//if (!gladLoadGLES2(glfwGetProcAddress)) return -3;
	glfwSwapInterval(0);

	logic->GLInit();
	logic->Init();
	while (!glfwWindowShouldClose(wnd)) {
		glfwPollEvents();
		logic->GLUpdateBegin();
		logic->Update((float)glfwGetTime());
		logic->GLUpdateEnd();
		glfwSwapBuffers(wnd);
	}

	return 0;
}
