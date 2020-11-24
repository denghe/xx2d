#include "xx_ptr.h"
#include "glfw/glfw3.h"
#include "glad/glad.h"

#include "linmath.h"
#ifdef _MSC_VER
#undef inline
#endif

#define var decltype(auto)

//#include "gltest1.hpp"
//#include "gltest2.hpp"
#include "gltest3.hpp"
//#include "gltest4.hpp"


GLFWwindow* wnd = nullptr;
inline int width = 0;
inline int height = 0;

int main()
{
	glfwSetErrorCallback([](int error, const char* description)
		{
			throw new std::exception(description, error);
		});

	if (!glfwInit()) return -1;
	auto sg_glfw = xx::MakeScopeGuard([] { glfwTerminate(); });

	wnd = glfwCreateWindow(1280, 720, "xx2dtest1", nullptr, nullptr);
	if (!wnd) return -2;
	auto sg_wnd = xx::MakeScopeGuard([&] { glfwDestroyWindow(wnd); });

	//glfwSetWindowUserPointer(wnd, ) for store wnd handler class pointer

	glfwSetKeyCallback(wnd, [](GLFWwindow* wnd, int key, int scancode, int action, int mods)
		{
			assert(wnd == ::wnd);
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(wnd, GLFW_TRUE);
			}
		});

	glfwSetFramebufferSizeCallback(wnd, [](GLFWwindow* wnd, int width, int height)
		{
			assert(wnd == ::wnd);
			::width = width;
			::height = height;
		});
	glfwGetFramebufferSize(wnd, &width, &height);

	glfwMakeContextCurrent(wnd);
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) return -3;
	glfwSwapInterval(1);

	//GLTest1 gltest;
	//GLTest2 gltest;
	GLTest3 gltest;
	//GLTest4 gltest;
	while (!glfwWindowShouldClose(wnd))
	{
		gltest.Update(width, height, (float)glfwGetTime());

		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	return 0;
}
