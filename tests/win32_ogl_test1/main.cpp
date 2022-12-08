// https://learnopengl-cn.github.io/01%20Getting%20started/03%20Hello%20Window/

#include "../../3rd/xxlib/xx_helpers.h"

#define GLAD_MALLOC(sz)       malloc(sz)
#define GLAD_FREE(ptr)        free(ptr)
#define GLAD_GL_IMPLEMENTATION
#include "../../3rd/glad/include/glad/glad.h"

#include "../../_bak/glfw/glfw3.h"
#pragma comment(lib, "../../_bak/glfw/glfw3.lib")

#ifndef NDEBUG
inline void CheckGLErrorAt(const char* file, int line, const char* func) {
    if (auto e = glGetError(); e != GL_NO_ERROR) {
        throw std::runtime_error(xx::ToStringFormat("OpenGL error: {0}, at {1}:{2}, {3}", e, file, line, func));
    }
}
#define CheckGLError() CheckGLErrorAt(__FILE__, __LINE__, __FUNCTION__)
#else
#define CheckGLError() ((void)0)
#endif

int main()
{
    if (!glfwInit()) return -1;
    auto sg_glfw = xx::MakeSimpleScopeGuard([] { glfwTerminate(); });

    glfwSetErrorCallback([](int error, const char* description) {
        throw new std::exception(description, error);
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    constexpr int W = 800, H = 600;

    auto window = glfwCreateWindow(W, H, "LearnOpenGL", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    auto sg_wnd = xx::MakeSimpleScopeGuard([&] { glfwDestroyWindow(window); });
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    while (glGetError() != GL_NO_ERROR) Sleep(1);

    glViewport(0, 0, W, H);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
