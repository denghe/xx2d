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

	glfwSetErrorCallback([](int error, const char* description) {
		throw new std::exception(description, error);
		});

	if (!glfwInit())
		return -1;
	auto sg_glfw = xx::MakeSimpleScopeGuard([] { glfwTerminate(); });

	wnd = glfwCreateWindow(logic->w, logic->h, "xx2dtest1", nullptr, nullptr);
	if (!wnd)
		return -2;
	auto sg_wnd = xx::MakeSimpleScopeGuard([&] { glfwDestroyWindow(wnd); });

	glfwSetKeyCallback(wnd, [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(wnd, GLFW_TRUE);
		}
		// todo: set key to logic
	});

	//glfwSetCharCallback(CORE.Window.handle, CharCallback);
	//glfwSetScrollCallback(CORE.Window.handle, MouseScrollCallback);
	//glfwSetCursorEnterCallback(CORE.Window.handle, CursorEnterCallback);

	glfwSetCursorPosCallback(wnd, [](GLFWwindow* wnd, double x, double y) {
		::logic->mousePosition = { (float)x - ::logic->w / 2, ::logic->h / 2 - (float)y };
	});

	glfwSetMouseButtonCallback(wnd, [](GLFWwindow* wnd, int button, int action, int mods) {
		::logic->mouseButtonStates[button] = action;
	});

	glfwSetFramebufferSizeCallback(wnd, [](GLFWwindow* wnd, int w, int h) {
		::logic->SetWH(w, h);
	});
	glfwGetFramebufferSize(wnd, &width, &height);
	::logic->SetWH(width, height);

	glfwMakeContextCurrent(wnd);

	glfwSetInputMode(wnd, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
	glfwSwapInterval(0);	// no v-sync by default

	if (!gladLoadGL(glfwGetProcAddress))
		return -3;

	while (auto e = glGetError()) {};	// cleanup glfw3 error

	logic->EngineInit();
	glfwSetCursorPos(wnd, logic->mousePosition.x, logic->mousePosition.y);

	logic->Init();
	while (!glfwWindowShouldClose(wnd)) {
		glfwPollEvents();
		logic->EngineUpdateBegin();

		logic->Update((float)glfwGetTime());

		logic->EngineUpdateEnd();
		glfwSwapBuffers(wnd);
	}

	logic->EngineDestroy();
	logic.Reset();

	return 0;
}
