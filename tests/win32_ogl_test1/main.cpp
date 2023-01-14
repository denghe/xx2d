// https://learnopengl-cn.github.io/01%20Getting%20started/03%20Hello%20Window/

#include "glutils.h"

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

    constexpr int W = 1920, H = 1080;

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

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });


    std::string_view vsSrc = R"#(#version 300 es
layout (location = 0) in vec2 pos;
void main() {
    gl_Position = vec4(pos.x, pos.y, 1.0, 1.0);
}
)#";

    std::string_view fsSrc = R"#(#version 300 es
precision mediump float;
out vec4 oColor;
void main() {
    oColor = vec4( 1.0, 1.0, 1.0, 1.0 );
}
)#";

    auto v = LoadGLVertexShader({ vsSrc });
    auto f = LoadGLFragmentShader({ fsSrc });
    auto p = LinkGLProgram(v, f);

    float vertices[] = {
    -0.5f, -0.5f,
    -0.5f, 0.5f,
    0.5f, 0.5f,
    0.5f, -0.5f,

    -0.4f, -0.4f,
    -0.4f, 0.4f,
    0.4f, 0.4f,
    0.4f, -0.4f,
    };
    unsigned short indices[] = {
        0, 1, 2, 3, 65535, 4, 5, 6, 7, 65535
    };
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(65535);
    glPointSize(5);

    GLVertexArrays va;
    GLBuffer vb, ib;

    glGenVertexArrays(1, &va.Ref());
    glBindVertexArray(va);

    glGenBuffers(1, (GLuint*)&vb);
    glGenBuffers(1, (GLuint*)&ib);

    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CheckGLError();


    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            break;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, W, H);

        glUseProgram(p);
        glBindVertexArray(va);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

        glDrawElements(GL_LINE_LOOP, _countof(indices), GL_UNSIGNED_SHORT, 0);
        glDrawElements(GL_POINTS, _countof(indices), GL_UNSIGNED_SHORT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
