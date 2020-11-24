#include "xx_ptr.h"
#include "xx_chrono.h"
#include "glew/glew.h"
#include "glfw/glfw3.h"

struct Env;
inline static Env* gEnv = nullptr;
struct Env {
	int width = 1280;
	int height = 720;
	bool fullScreen = false;
	std::string title = "gles3test2 fps: ";
	std::filesystem::path rootPath;

	int lastErrorNumber = 0;
	std::string lastErrorMessage;

	GLFWmonitor* monitor = nullptr;
	GLFWwindow* wnd = nullptr;

	float w = 0;
	float h = 0;
	double beginTime = 0;
	double lastTime = 0;
	float delta = 0;
	uint64_t numFrames = 0;

	inline void onGLFWError(int num, const char* msg) {
		lastErrorNumber = num;
		lastErrorMessage = msg;
	}
	inline void onMouseButtonCallback(int button, int action, int modify) {
		std::cout << button << " " << action << " " << modify << std::endl;
	}
	inline void onCursorPosCallback(double x, double y) {
		std::cout << x << " " << y << std::endl;
	}
	inline void onScrollCallback(double x, double y) {
		std::cout << x << " " << y << std::endl;
	}
	inline void onKeyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(wnd, GLFW_TRUE);
		}
	}
	inline void onCharCallback(unsigned int character) {}
	inline void onWindowPosCallback(int x, int y) {}
	inline void onFramebufferSizeCallback(int w, int h) {
		width = w;
		height = h;
	}
	inline void onWindowSizeCallback(int w, int h) {}
	inline void onWindowIconifyCallback(int iconified) {}
	inline void onWindowFocusCallback(int focused) {}

	Env() {
		gEnv = this;
	}
	~Env() {
		gEnv = nullptr;
	}

	inline int Run() {
		/************************************************************************************************/
		// 初始化窗口容器

		glfwSetErrorCallback([](auto n, auto m) { gEnv->onGLFWError(n, m); });
		if (glfwInit() == GLFW_FALSE) return __LINE__;
		auto sg = xx::MakeScopeGuard([] {
			glfwTerminate();
			});

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
		wnd = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
		if (!wnd) return __LINE__;
		auto sg2 = xx::MakeScopeGuard([this] {
			glfwDestroyWindow(wnd);
			wnd = nullptr;
			});

		glfwMakeContextCurrent(wnd);
		glfwGetWindowSize(wnd, &width, &height);
		glfwSwapInterval(1);

		// 回调转到成员函数
		glfwSetMouseButtonCallback(wnd, [](auto w, auto b, auto a, auto m) { gEnv->onMouseButtonCallback(b, a, m); });
		glfwSetCursorPosCallback(wnd, [](auto w, auto x, auto y) { gEnv->onCursorPosCallback(x, y); });
		glfwSetScrollCallback(wnd, [](auto w, auto x, auto y) { gEnv->onScrollCallback(x, y); });
		glfwSetCharCallback(wnd, [](auto w, auto c) { gEnv->onCharCallback(c); });
		glfwSetKeyCallback(wnd, [](auto w, auto k, auto s, auto a, auto m) { gEnv->onKeyCallback(k, s, a, m); });
		glfwSetWindowPosCallback(wnd, [](auto w, auto x, auto y) { gEnv->onWindowPosCallback(x, y); });
		glfwSetFramebufferSizeCallback(wnd, [](auto w_, auto w, auto h) { gEnv->onFramebufferSizeCallback(w, h); });
		glfwSetWindowSizeCallback(wnd, [](auto w_, auto w, auto h) { gEnv->onWindowSizeCallback(w, h); });
		glfwSetWindowIconifyCallback(wnd, [](auto w, auto i) { gEnv->onWindowIconifyCallback(i); });
		glfwSetWindowFocusCallback(wnd, [](auto w, auto f) { gEnv->onWindowFocusCallback(f); });

		/************************************************************************************************/
		// 初始化 opengl

		if (int r = glewInit()) {
			lastErrorNumber = r;
			lastErrorMessage = (char*)glewGetErrorString(r);
			return lastErrorNumber;
		}

		if (glGenFramebuffers == nullptr) {
			lastErrorNumber = __LINE__;
			lastErrorMessage = "glGenFramebuffers == nullptr";
			return lastErrorNumber;
		}

		GLInit();

		/************************************************************************************************/
		// 开始帧循环

		lastTime = xx::NowSteadyEpochSeconds();
		beginTime = lastTime - 0.0000000000000001;
		while (!glfwWindowShouldClose(wnd)) {
			delta = (float)xx::NowSteadyEpochSeconds(lastTime);
			++numFrames;
			glfwSetWindowTitle(wnd, (title + std::to_string((double)numFrames / (lastTime - beginTime))).c_str());
			glfwPollEvents();

			glDepthMask(true);
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDepthMask(false);

			Update();

			glfwSwapBuffers(wnd);
		}

		return 0;
	}


	inline void GLInit() {
		// todo
	}
	inline void Update() {
		// todo
	}
};

int main() {
	Env e;
	if (int r = e.Run()) {
		std::cout << "e.Run() = " << e.lastErrorNumber << ", " << e.lastErrorMessage << std::endl;
	}
}
