#include"xx2d.h"
#include <GLFW/glfw3.h>

namespace xx {

	int GameLooperBase::Run(std::string const& wndTitle) {
		xx::engine.Init();

#ifdef _WIN32
		SetConsoleOutputCP(65001);
#endif

#ifndef __EMSCRIPTEN__
		std::cout << "working dir = " << xx::GetPath_Current().string() << std::endl;
#endif

		glfwSetErrorCallback([](int error, const char* description) {
			throw std::logic_error{ std::string(description) + "   error number = " + std::to_string(error) };
			});

		if (!glfwInit())
			return -1;
#ifndef __EMSCRIPTEN__
		auto sg_glfw = xx::MakeSimpleScopeGuard([] { glfwTerminate(); });
#endif

		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_DEPTH_BITS, 0);
		glfwWindowHint(GLFW_STENCIL_BITS, 0);

		auto wnd = glfwCreateWindow(xx::engine.w, xx::engine.h, wndTitle.c_str(), nullptr, nullptr);
		if (!wnd)
			return -2;
#ifndef __EMSCRIPTEN__
		auto sg_wnd = xx::MakeSimpleScopeGuard([&] { glfwDestroyWindow(wnd); });
#else
		this->wnd = wnd;
#endif

		// reference from raylib rcore.c
		glfwSetKeyCallback(wnd, [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
			if (key < 0) return;    // macos fn key == -1
			xx::engine.kbdStates[key] = action;
			});

		glfwSetCharCallback(wnd, [](GLFWwindow* wnd, unsigned int key) {
				xx::engine.kbdInputs.push_back(key);
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

		int width{}, height{};
		glfwGetFramebufferSize(wnd, &width, &height);
		xx::engine.SetWH(width, height);

		glfwMakeContextCurrent(wnd);

#ifndef __EMSCRIPTEN__
		glfwSetInputMode(wnd, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
		glfwSwapInterval(0);	// no v-sync by default

		if (!gladLoadGL(glfwGetProcAddress))
			return -3;

		while (auto e = glGetError()) {};	// cleanup glfw3 error

		glfwSetCursorPos(wnd, xx::engine.mousePosition.x, xx::engine.mousePosition.y);
#endif

		// ***************************************************************************************
		//  user code call here
		// ***************************************************************************************

		Init();										// looper init1

		xx::engine.GLInit(wnd);

		AfterGLInit();								// looper init2

#ifndef __EMSCRIPTEN__
		int r{};
		while (!glfwWindowShouldClose(wnd)) {
			glfwPollEvents();
			xx::engine.UpdateBegin();

			r = this->Update();						// looper update

			xx::engine.UpdateEnd();
			glfwSwapBuffers(wnd);

			if (r) break;
		}

		this->Deinit();								// looper deinit

		xx::engine.Destroy();

		return r;
#else
		emscripten_request_animation_frame_loop([](double time, void *userData)->EM_BOOL {
			auto self = (GameLooperBase*)userData;

			glfwPollEvents();
			xx::engine.UpdateBegin();

			auto r = self->Update();						// looper update

			xx::engine.UpdateEnd();
			glfwSwapBuffers((GLFWwindow *)self->wnd);

			return r ? EM_FALSE : EM_TRUE;
		}, this);
		return 0;
#endif
	}
}
