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

	if (!glfwInit()) return -1;
	auto sg_glfw = xx::MakeSimpleScopeGuard([] { glfwTerminate(); });

	glfwDefaultWindowHints();
	//glfwxSetParent((HWND)0);
	//glfwWindowHint(GLFW_RED_BITS, 8);
	//glfwWindowHint(GLFW_GREEN_BITS, 8);
	//glfwWindowHint(GLFW_BLUE_BITS, 8);
	//glfwWindowHint(GLFW_ALPHA_BITS, 8);
	//glfwWindowHint(GLFW_DEPTH_BITS, 24);
	//glfwWindowHint(GLFW_STENCIL_BITS, 8);
	//glfwWindowHint(GLFW_SAMPLES, 0);
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API); // GLFW_OPENGL_ES_API
	//glfwWindowHint(GLFW_AUX_BUFFERS, 0);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
	glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
	glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwSetJoystickCallback(nullptr);
	

	wnd = glfwCreateWindow(logic->w, logic->h, "xx2dtest1", nullptr, nullptr);
	if (!wnd) return -3;
	auto sg_wnd = xx::MakeSimpleScopeGuard([&] { glfwDestroyWindow(wnd); });

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

	glfwMakeContextCurrent(wnd);

	glfwSetInputMode(wnd, GLFW_LOCK_KEY_MODS, GLFW_TRUE);    // Enable lock keys modifiers (CAPS, NUM)
	glfwSwapInterval(0);	// No V-Sync by default

	if (!gladLoadGL(glfwGetProcAddress)) return -4;
	//if (!gladLoadGLES2(glfwGetProcAddress)) return -4;


	GLint numExt = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
	bool supportASTC = GLAD_GL_KHR_texture_compression_astc_hdr && GLAD_GL_KHR_texture_compression_astc_ldr;
	bool supportDXT = GLAD_GL_EXT_texture_compression_s3tc;  // Texture compression: DXT
	bool supportETC2 = GLAD_GL_ARB_ES3_compatibility;        // Texture compression: ETC2/EAC
	std::cout << glGetString(GL_VERSION) << std::endl;

	// 当前情况为 刚开始始终会产生 1282 的错误码. 清掉
	while (auto e = glGetError()) {
		std::cout << "glGetError() == " << e << std::endl;
	};

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
