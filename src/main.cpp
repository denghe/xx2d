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

	glfwMakeContextCurrent(wnd);

	glfwSetInputMode(wnd, GLFW_LOCK_KEY_MODS, GLFW_TRUE);    // Enable lock keys modifiers (CAPS, NUM)
	glfwSwapInterval(0);	// No V-Sync by default

	if (!gladLoadGL(glfwGetProcAddress))
		return -3;

	while (auto e = glGetError()) {};	// cleanup glfw3 error

	logic->EngineInit();
	logic->Init();
	while (!glfwWindowShouldClose(wnd)) {
		glfwPollEvents();
		logic->EngineUpdateBegin();
		logic->Update((float)glfwGetTime());
		logic->EngineUpdateEnd();
		glfwSwapBuffers(wnd);
	}
	logic->EngineDestroy();

	return 0;
}
