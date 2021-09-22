// demo：快速画大量 quad，使用 OpenGL Core Profile 3.3

#include <cstdio>
#include <cstddef>
#include <cmath>
#include <vector>
#include <chrono>

#include <Windows.h>

#include "glad.h"
#include <GLFW/glfw3.h>

#pragma comment (lib, "opengl32.lib")

constexpr float PI = 3.1415926f;

#ifdef _DEBUG
void check_gl_error_at(const char* file, int line, const char* func)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error: 0x%04x, at %s:%d, %s\n", err, file, line, func);
        abort();
    }
}

#define check_gl_error() check_gl_error_at(__FILE__, __LINE__, __FUNCTION__)
#else
#define check_gl_error() ((void)0)
#endif

GLuint create_shader(GLenum type, const char* glsl_source)
{
    std::vector<const char*> str_of_each_line;
    std::vector<GLint> length_of_each_line;
    const char* p_str = glsl_source;
    for (;;)
    {
        str_of_each_line.push_back(p_str);

        const char* end = strchr(p_str, '\n');
        if (end != nullptr)
        {
            length_of_each_line.push_back(GLint(end - p_str + 1));
            p_str = end + 1;
        }
        else
        {
            length_of_each_line.push_back(GLint(strlen(glsl_source)));
            break;
        }
    }

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, GLsizei(str_of_each_line.size()), str_of_each_line.data(), length_of_each_line.data());
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<char> log(size_t(log_length) + 1);
        glGetShaderInfoLog(shader, log_length, &log_length, log.data());
        log[log_length] = '\0';
        printf("compile shader error: %s\n", log.data());
        abort();
    }

    return shader;
}

GLuint init_demo_program()
{
    const char* vertex_shader_source =
        "#version 300 es\n"
        "\n"
        "uniform vec4 u_projection;\n"
        "in vec4 in_offset_texcoord;\n"
        "in vec4 in_center_scale_rotate;\n"
        "out vec2 v_texcoord;\n"
        "void main() {\n"
        "    vec2 offset = in_offset_texcoord.xy;\n"
        "    vec2 texcoord = in_offset_texcoord.zw;\n"
        "    vec2 center = in_center_scale_rotate.xy;\n"
        "    float scale = in_center_scale_rotate.z;\n"
        "    float rotate = in_center_scale_rotate.w;\n"
        "\n"
        "    float cos_theta = cos(rotate);\n"
        "    float sin_theta = sin(rotate);\n"
        "    vec2 v1 = offset * scale;\n"
        "    vec2 v2 = vec2(\n"
        "       dot(v1, vec2(cos_theta, sin_theta)),\n"
        "       dot(v1, vec2(-sin_theta, cos_theta))\n"
        "       );\n"
        "    vec2 v3 = (v2 + center) * u_projection.xy + u_projection.zw;\n"
        "\n"
        "    v_texcoord = texcoord;\n"
        "    gl_Position = vec4(v3.x, v3.y, 0.0, 1.0);\n"
        "}\n"
        ;
    const GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_source);

    const char* fragment_shader_source =
        "#version 300 es\n"
        "precision mediump float;\n"
        "\n"
        "uniform sampler2D u_sampler;\n"
        "in vec2 v_texcoord;\n"
        "out vec4 out_color;"
        "void main() {\n"
        "    out_color = texture(u_sampler, v_texcoord);\n"
        "}\n"
        ;
    const GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<char> log(size_t(log_length) + 1);
        glGetProgramInfoLog(program, log_length, &log_length, log.data());
        log[log_length] = '\0';
        printf("program link error: %s\n", log.data());
        abort();
    }

    return program;
}

GLuint init_demo_texture()
{
    constexpr uint32_t texture_size = 128;
    constexpr GLfloat center_xy = float(texture_size) * 0.5f;
    constexpr GLfloat radius = float(texture_size) * 0.5f;

    std::vector<GLubyte> pixels(texture_size * texture_size * 4);
    for (uint32_t iy = 0; iy < texture_size; ++iy) {
        GLfloat y = GLfloat(iy);
        for (uint32_t ix = 0; ix < texture_size; ++ix) {
            GLfloat x = GLfloat(ix);

            GLubyte* pix = &pixels[(size_t(iy) * texture_size + ix) * 4];

            GLfloat diff_x = y - center_xy;
            GLfloat diff_y = x - center_xy;
            GLfloat theta = atan2(diff_y, diff_x);

            GLfloat h = atan2(diff_y, diff_x) * (3.0f / PI) + 3.0f;
            GLfloat s = sqrt(diff_x * diff_x + diff_y * diff_y) / radius;
            GLfloat v = 1.0f;

            GLfloat color_r, color_g, color_b;
            if (s != 0.0f)
            {
                int i = int(h);
                if (i > 5) i = 5;

                GLfloat f = h - (GLfloat)i;
                GLfloat a = v * (1.0f - s);
                GLfloat b = v * (1.0f - s * f);
                GLfloat c = v * (1.0f - s * (1.0f - f));
                switch (i)
                {
                case 0: color_r = v; color_g = c; color_b = a; break;
                case 1: color_r = b; color_g = v; color_b = a; break;
                case 2: color_r = a; color_g = v; color_b = c; break;
                case 3: color_r = a; color_g = b; color_b = v; break;
                case 4: color_r = c; color_g = a; color_b = v; break;
                case 5: color_r = v; color_g = a; color_b = b; break;
                default: abort();
                }

                if (color_r < 0.0f) color_r = 0.0f; else if (color_r > 1.0f) color_r = 1.0f;
                if (color_g < 0.0f) color_g = 0.0f; else if (color_g > 1.0f) color_g = 1.0f;
                if (color_b < 0.0f) color_b = 0.0f; else if (color_b > 1.0f) color_b = 1.0f;
            }
            else
            {
                color_r = color_g = color_b = 1.0f;
            }

            pix[0] = GLubyte(color_r * 255.0f);
            pix[1] = GLubyte(color_g * 255.0f);
            pix[2] = GLubyte(color_b * 255.0f);
            pix[3] = (s >= 1.0f ? 0x00 : 0xFF);
        }
    }

    GLuint texture_obj;
    glGenTextures(1, &texture_obj);
    glBindTexture(GL_TEXTURE_2D, texture_obj);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture_size, texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texture_obj;
}

class DemoQuadsIA
{
public:
    struct QuadData_center_scale_rotate // 直接传给 GPU 用
    {
        GLfloat x;
        GLfloat y;
        GLfloat scale;
        GLfloat rotate;
    };

    struct QuadData_offset_texcoord     // 直接传给 GPU 用
    {
        GLfloat x;
        GLfloat y;
        GLfloat u;
        GLfloat v;
    };

    void init(GLuint program, GLuint quad_count);
    QuadData_center_scale_rotate* map_buffer();
    void unmap_buffer();
    void draw();

private:
    static constexpr int BUFFER_COUNT = 1;

    GLuint m_quad_count;
    int m_current_position;
    GLint m_attribloc_offset_texcoord;
    GLint m_attribloc_center_scale_rotate;
    GLuint m_vao;
    GLuint m_vbo_per_instance[BUFFER_COUNT];
    GLuint m_vbo_shared;
};

void DemoQuadsIA::init(GLuint program, GLuint quad_count)
{
    m_quad_count = quad_count;
    m_current_position = -1;

    m_attribloc_offset_texcoord = glGetAttribLocation(program, "in_offset_texcoord");
    m_attribloc_center_scale_rotate = glGetAttribLocation(program, "in_center_scale_rotate");

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(BUFFER_COUNT, m_vbo_per_instance);
    for (int i = 0; i < BUFFER_COUNT; ++i)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_per_instance[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData_center_scale_rotate) * quad_count, nullptr, GL_STREAM_DRAW);
    }

    QuadData_offset_texcoord offset_and_texcoord[6] = {
        { -0.5f, -0.5f, 0.0f, 0.0f },
        { 0.5f, -0.5f, 1.0f, 0.0f },
        { 0.5f, 0.5f, 1.0f, 1.0f },

        { 0.5f, 0.5f, 1.0f, 1.0f },
        { -0.5f, 0.5f, 0.0f, 1.0f },
        { -0.5f, -0.5f, 0.0f, 0.0f },
    };

    glGenBuffers(1, &m_vbo_shared);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_shared);
    glBufferData(GL_ARRAY_BUFFER, sizeof(offset_and_texcoord), offset_and_texcoord, GL_STATIC_DRAW);
    check_gl_error();
}

DemoQuadsIA::QuadData_center_scale_rotate* DemoQuadsIA::map_buffer()
{
    ++m_current_position;
    if (m_current_position == BUFFER_COUNT)
    {
        m_current_position = 0;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_per_instance[m_current_position]);
    return static_cast<QuadData_center_scale_rotate*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(QuadData_center_scale_rotate) * m_quad_count,
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
}

void DemoQuadsIA::unmap_buffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_per_instance[m_current_position]);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void DemoQuadsIA::draw()
{
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_shared);
    check_gl_error();
    glVertexAttribPointer(m_attribloc_offset_texcoord, 4, GL_FLOAT, GL_FALSE, sizeof(QuadData_offset_texcoord), 0);
    check_gl_error();
    glEnableVertexAttribArray(m_attribloc_offset_texcoord);
    check_gl_error();

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_per_instance[m_current_position]);
    check_gl_error();
    glVertexAttribPointer(m_attribloc_center_scale_rotate, 4, GL_FLOAT, GL_FALSE, sizeof(QuadData_center_scale_rotate), 0);
    check_gl_error();
    glVertexAttribDivisor(m_attribloc_center_scale_rotate, 1);
    check_gl_error();
    glEnableVertexAttribArray(m_attribloc_center_scale_rotate);
    check_gl_error();

#if 0
    const GLuint instance_per_draw = 1000;
    for (GLuint i = 0; i < m_quad_count; i += instance_per_draw)
    {
        GLuint instance = instance_per_draw;
        if (i + instance_per_draw > m_quad_count)
        {
            instance = m_quad_count - i;
        }
        glVertexAttribPointer(m_attribloc_center_scale_rotate, 4, GL_FLOAT, GL_FALSE, sizeof(QuadData_center_scale_rotate),
            static_cast<char*>(nullptr) + sizeof(QuadData_center_scale_rotate) * i);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instance);
    }
#else
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_quad_count);
#endif
    check_gl_error();
}

int main(void)
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    constexpr int window_width = 1920;
    constexpr int window_height = 1080;

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Hello World", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    gladLoadGL();

    const char* gl_version = (const char*)glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", gl_version);

    GLint mask = 0;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
    if (mask & GL_CONTEXT_CORE_PROFILE_BIT)
    {
        printf("OpenGL is init to core profile.\n");
    }
    if (mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
    {
        printf("OpenGL is init to compatibility profile.\n");
    }

    check_gl_error();
    GLuint program = init_demo_program();
    check_gl_error();
    GLuint texture = init_demo_texture();
    check_gl_error();

    glUseProgram(program);

    GLint uloc_sampler = glGetUniformLocation(program, "u_sampler");
    glUniform1i(uloc_sampler, 0);
    check_gl_error();

    constexpr int design_resolution_x = 1920;
    constexpr int design_resolution_y = 1080;
    constexpr int quad_count = 1000000;

    GLint uloc_projection = glGetUniformLocation(program, "u_projection");
    glUniform4f(uloc_projection,
        2.0f / design_resolution_x,
        2.0f / design_resolution_y,
        -1.0f,
        -1.0f);
    check_gl_error();

    DemoQuadsIA ia;
    ia.init(program, quad_count);
    check_gl_error();

    struct LogicQuad
    {
        GLfloat x;
        GLfloat y;
        GLfloat scale;
        GLfloat rotate;
        GLfloat speed_x;
        GLfloat speed_y;
        GLfloat speed_scale;
        GLfloat speed_rotate;
    };

    auto random_of_range = [](GLfloat min, GLfloat max)
    {
        return min + (max - min) * (GLfloat(rand()) / RAND_MAX);
    };

    auto update_for_speed = [](GLfloat* value, GLfloat* speed, GLfloat delta_time, GLfloat min, GLfloat max)
    {
        *value += *speed * delta_time;
        if (*value > max && *speed > 0.0f || *value < min && *speed < 0.0f)
        {
            *speed = -*speed;
        }
    };

    constexpr GLfloat min_x = 0.0f;
    constexpr GLfloat max_x = GLfloat(design_resolution_x);
    constexpr GLfloat min_y = 0.0f;
    constexpr GLfloat max_y = GLfloat(design_resolution_y);
    constexpr GLfloat min_scale = 15.0f;
    constexpr GLfloat max_scale = 25.0f;
    constexpr GLfloat min_rotate = -PI * 5;
    constexpr GLfloat max_rotate = PI * 5;

    std::vector<LogicQuad> logic_quads(quad_count);
    for (int i = 0; i < quad_count; ++i)
    {
        auto& q = logic_quads[i];
        q.x = random_of_range(min_x, max_x);
        q.y = random_of_range(min_y, max_y);
        q.scale = random_of_range(min_scale, max_scale);
        q.rotate = random_of_range(min_rotate, max_rotate);

        q.speed_x = random_of_range(50.0f, 80.0f) * GLfloat(rand() % 2 * 2 - 1);
        q.speed_y = random_of_range(50.0f, 80.0f) * GLfloat(rand() % 2 * 2 - 1);
        q.speed_scale = random_of_range(3.0f, 5.0f) * GLfloat(rand() % 2 * 2 - 1);
        q.speed_rotate = random_of_range(0.5f * PI, 0.8f * PI) * GLfloat(rand() % 2 * 2 - 1);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LARGE_INTEGER qpc_freq, qpc_last, qpc_curr;
    QueryPerformanceFrequency(&qpc_freq);
    QueryPerformanceCounter(&qpc_last);

    int frame_count = 0;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        QueryPerformanceCounter(&qpc_curr);

        GLfloat delta_time = GLfloat((double)(qpc_curr.QuadPart - qpc_last.QuadPart) / (double)qpc_freq.QuadPart);
        ++frame_count;
        printf("frame: %d, quad count = %d, delta_time = %.5f, fps = %.2f\n", frame_count, quad_count, delta_time, 1.0f / delta_time);
        qpc_last = qpc_curr;

        auto* buf = ia.map_buffer();
        check_gl_error();

        // #pragma omp parallel for
        for (int i = 0; i < quad_count; ++i)
        {
            auto& logic_quad = logic_quads[i];
            auto& render_quad = buf[i];

            // if (frame_count < 100)
            {
                update_for_speed(&logic_quad.x, &logic_quad.speed_x, delta_time, min_x, max_x);
                update_for_speed(&logic_quad.y, &logic_quad.speed_y, delta_time, min_y, max_y);
                update_for_speed(&logic_quad.scale, &logic_quad.speed_scale, delta_time, min_scale, max_scale);
                update_for_speed(&logic_quad.rotate, &logic_quad.speed_rotate, delta_time, min_rotate, max_rotate);
            }

            render_quad.x = logic_quad.x;
            render_quad.y = logic_quad.y;
            render_quad.scale = logic_quad.scale * 0.5f;
            render_quad.rotate = logic_quad.rotate;
        }
        ia.unmap_buffer();
        check_gl_error();

        glBindTexture(GL_TEXTURE_2D, texture);
        check_gl_error();
        ia.draw();
        check_gl_error();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}













//#include "xx_ptr.h"
//#include "glfw/glfw3.h"
//#include "glad/glad.h"
//
//#include "linmath.h"
//#ifdef _MSC_VER
//#undef inline
//#endif
//
//#define var decltype(auto)
//
////#include "gltest1.hpp"
////#include "gltest2.hpp"
//#include "gltest3.hpp"
////#include "gltest4.hpp"
////#include "gltest5.hpp"
//
//
//GLFWwindow* wnd = nullptr;
//inline int width = 0;
//inline int height = 0;
//
//int main()
//{
//	glfwSetErrorCallback([](int error, const char* description)
//		{
//			throw new std::exception(description, error);
//		});
//
//	if (!glfwInit()) return -1;
//	auto sg_glfw = xx::MakeScopeGuard([] { glfwTerminate(); });
//
//	wnd = glfwCreateWindow(1280, 720, "xx2dtest1", nullptr, nullptr);
//	if (!wnd) return -2;
//	auto sg_wnd = xx::MakeScopeGuard([&] { glfwDestroyWindow(wnd); });
//
//	//glfwSetWindowUserPointer(wnd, ) for store wnd handler class pointer
//
//	glfwSetKeyCallback(wnd, [](GLFWwindow* wnd, int key, int scancode, int action, int mods)
//		{
//			assert(wnd == ::wnd);
//			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//			{
//				glfwSetWindowShouldClose(wnd, GLFW_TRUE);
//			}
//		});
//
//	glfwSetFramebufferSizeCallback(wnd, [](GLFWwindow* wnd, int width, int height)
//		{
//			assert(wnd == ::wnd);
//			::width = width;
//			::height = height;
//		});
//	glfwGetFramebufferSize(wnd, &width, &height);
//
//	glfwMakeContextCurrent(wnd);
//	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) return -3;
//	glfwSwapInterval(1);
//
//	//GLTest1 gltest;
//	//GLTest2 gltest;
//	GLTest3 gltest;
//	//GLTest4 gltest;
//	//GLTest5 gltest;
//
//	while (!glfwWindowShouldClose(wnd))
//	{
//		gltest.Update(width, height, (float)glfwGetTime());
//
//		glfwSwapBuffers(wnd);
//		glfwPollEvents();
//	}
//
//	return 0;
//}
