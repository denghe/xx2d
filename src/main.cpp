#include"xx2d_pch.h"
#include "game_looper.h"		// user code ref here
#include <GLFW/glfw3.h>

GLFWwindow* wnd = nullptr;
inline int width = 0;
inline int height = 0;

int main() {
	xx::engine.EngineInit();

#ifdef _WIN32
	SetConsoleOutputCP(65001);
#endif

	std::cout << "working dir = " << xx::GetPath_Current().string() << std::endl;

	glfwSetErrorCallback([](int error, const char* description) {
		throw std::logic_error{std::string(description) + "   error number = " + std::to_string(error)};
	});

	if (!glfwInit())
		return -1;
	auto sg_glfw = xx::MakeSimpleScopeGuard([] { glfwTerminate(); });

	wnd = glfwCreateWindow(xx::engine.w, xx::engine.h, "xx2d game engine", nullptr, nullptr);
	if (!wnd)
		return -2;
	auto sg_wnd = xx::MakeSimpleScopeGuard([&] { glfwDestroyWindow(wnd); });

	// reference from raylib rcore.c
	glfwSetKeyCallback(wnd, [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
        if (key < 0) return;    // macos fn key == -1
		xx::engine.kbdStates[key] = action;
	});

	glfwSetCharCallback(wnd, [](GLFWwindow* wnd, unsigned int key) {
		if (xx::engine.kbdInputs.size() < 512) {
			xx::engine.kbdInputs.push_back(key);
		}
	});

	//glfwSetScrollCallback(wnd, MouseScrollCallback);
	//glfwSetCursorEnterCallback(wnd, CursorEnterCallback);

	glfwSetCursorPosCallback(wnd, [](GLFWwindow* wnd, double x, double y) {
		xx::engine.mousePosition = { (float)x - xx::engine.w / 2, xx::engine.h / 2 - (float)y };
	});

	glfwSetMouseButtonCallback(wnd, [](GLFWwindow* wnd, int button, int action, int mods) {
		xx::engine.mbtnStates[button] = action;
	});

	glfwSetFramebufferSizeCallback(wnd, [](GLFWwindow* wnd, int w, int h) {
		xx::engine.SetWH(w, h);
	});
	glfwGetFramebufferSize(wnd, &width, &height);
	xx::engine.SetWH(width, height);

	glfwMakeContextCurrent(wnd);

	glfwSetInputMode(wnd, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
	glfwSwapInterval(0);	// no v-sync by default

	if (!gladLoadGL(glfwGetProcAddress))
		return -3;

	while (auto e = glGetError()) {};	// cleanup glfw3 error

	xx::engine.EngineGLInit(wnd);
	glfwSetCursorPos(wnd, xx::engine.mousePosition.x, xx::engine.mousePosition.y);

	// ***************************************************************************************
	//  user code call here
	// ***************************************************************************************

	auto o = xx::Make<GameLooper>();	// user class
	o->Init();
	while (!glfwWindowShouldClose(wnd)) {
		glfwPollEvents();
		xx::engine.EngineUpdateBegin();

		if (o->Update()) break;

		xx::engine.EngineUpdateEnd();
		glfwSwapBuffers(wnd);
	}

	o.Reset();
	xx::engine.EngineDestroy();

	return 0;
}
