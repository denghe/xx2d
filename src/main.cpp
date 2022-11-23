#include"pch.h"
#include "logic.h"

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

	if (!glfwInit()) return -1;
	auto sg_glfw = xx::MakeSimpleScopeGuard([] { glfwTerminate(); });

	wnd = glfwCreateWindow(logic->w, logic->h, "xx2dtest1", nullptr, nullptr);
	if (!wnd) return -2;
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
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) return -3;
	glfwSwapInterval(0);

	logic->GLInit();
	while (!glfwWindowShouldClose(wnd)) {
		glfwPollEvents();
		logic->Update((float)glfwGetTime());
		glfwSwapBuffers(wnd);
	}

	return 0;
}
