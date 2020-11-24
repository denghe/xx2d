#include "xx_ptr.h"
#include "xx_chrono.h"
#include "glew/glew.h"
#include "glfw/glfw3.h"


template<typename T>
struct Env {
	inline static Env* instance = nullptr;

	int width = 1280;
	int height = 720;
	int vsync = 1;	// 0: 0ff
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
	double fpsTimer = 0;

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
		instance = this;
	}
	virtual ~Env() {
		instance = nullptr;
	}

	inline int Run() {
		/************************************************************************************************/
		// 初始化窗口容器

		glfwSetErrorCallback([](auto n, auto m) { instance->onGLFWError(n, m); });
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
		glfwSwapInterval(vsync);

		// 回调转到成员函数
		glfwSetMouseButtonCallback(wnd, [](auto w, auto b, auto a, auto m) { instance->onMouseButtonCallback(b, a, m); });
		glfwSetCursorPosCallback(wnd, [](auto w, auto x, auto y) { instance->onCursorPosCallback(x, y); });
		glfwSetScrollCallback(wnd, [](auto w, auto x, auto y) { instance->onScrollCallback(x, y); });
		glfwSetCharCallback(wnd, [](auto w, auto c) { instance->onCharCallback(c); });
		glfwSetKeyCallback(wnd, [](auto w, auto k, auto s, auto a, auto m) { instance->onKeyCallback(k, s, a, m); });
		glfwSetWindowPosCallback(wnd, [](auto w, auto x, auto y) { instance->onWindowPosCallback(x, y); });
		glfwSetFramebufferSizeCallback(wnd, [](auto w_, auto w, auto h) { instance->onFramebufferSizeCallback(w, h); });
		glfwSetWindowSizeCallback(wnd, [](auto w_, auto w, auto h) { instance->onWindowSizeCallback(w, h); });
		glfwSetWindowIconifyCallback(wnd, [](auto w, auto i) { instance->onWindowIconifyCallback(i); });
		glfwSetWindowFocusCallback(wnd, [](auto w, auto f) { instance->onWindowFocusCallback(f); });

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

		if (int r = ((T*)this)->GLInit()) return r;

		/************************************************************************************************/
		// 开始帧循环

		beginTime = lastTime = xx::NowSteadyEpochSeconds();
		fpsTimer = lastTime + 0.5;

		while (!glfwWindowShouldClose(wnd)) {
			++numFrames;
			glfwPollEvents();

			delta = (float)xx::NowSteadyEpochSeconds(lastTime);
			((T*)this)->Update();

			glfwSwapBuffers(wnd);
		}

		return 0;
	}


	inline void DrawFps() {
		if (lastTime > fpsTimer) {
			fpsTimer = lastTime + 0.5;
			glfwSetWindowTitle(wnd, (title + std::to_string((double)numFrames / (lastTime - beginTime))).c_str());
		}
	}


	std::vector<GLchar const*> codes;
	std::vector<GLint> codeLens;

	// 加载 shader 代码段. 返回 0 代表出错
	inline GLuint LoadShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_) {
		// 前置检查
		if (!codes_.size() || !(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER)) {
			lastErrorNumber = __LINE__;
			lastErrorMessage = "LoadShader if (!codes.size() || !(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER))";
			return 0;
		}

		// 申请 shader 上下文. 返回 0 表示失败, 参数：GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER
		auto&& shader = glCreateShader(type);
		if (!shader) {
			lastErrorNumber = __LINE__;
			lastErrorMessage = "LoadShader error. glGetError() = " + std::to_string(glGetError());
			return 0;
		}
		auto&& sg = xx::MakeScopeGuard([&] {
			glDeleteShader(shader);
			});

		// 填充 codes & codeLens
		codes.resize(codes_.size());
		codeLens.resize(codes_.size());
		auto ss = codes_.begin();
		for (size_t i = 0; i < codes.size(); ++i) {
			codes[i] = (GLchar const*)ss[i].data();
			codeLens[i] = (GLint)ss[i].size();
		}

		// 参数：目标 shader, 代码段数, 段数组, 段长度数组
		glShaderSource(shader, (GLsizei)codes_.size(), codes.data(), codeLens.data());

		// 编译
		glCompileShader(shader);

		// 状态容器
		GLint r = GL_FALSE;

		// 查询是否编译成功
		glGetShaderiv(shader, GL_COMPILE_STATUS, &r);

		// 失败
		if (!r) {
			// 查询日志信息文本长度
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &r);
			if (r) {
				lastErrorNumber = __LINE__;
				// 复制错误文本
				lastErrorMessage.resize(r);
				glGetShaderInfoLog(shader, r, nullptr, lastErrorMessage.data());
			}
			return 0;
		}

		sg.Cancel();
		return shader;
	}

	inline GLuint LoadVertexShader(std::initializer_list<std::string_view>&& codes_) {
		return LoadShader(GL_VERTEX_SHADER, std::move(codes_));
	}
	inline GLuint LoadFragmentShader(std::initializer_list<std::string_view>&& codes_) {
		return LoadShader(GL_FRAGMENT_SHADER, std::move(codes_));
	}

	// 用 vs fs 链接出 program. 返回 0 表示失败
	inline GLuint LinkProgram(GLuint const& vs, GLuint const& fs) {
		// 创建一个 program. 返回 0 表示失败
		auto program = glCreateProgram();
		if (!program) {
			lastErrorNumber = __LINE__;
			lastErrorMessage = "LoadProgram error. glCreateProgram fail, glGetError() = ";
			lastErrorMessage.append(std::to_string(glGetError()));
			return 0;
		}
		auto sg = xx::MakeScopeGuard([&] { glDeleteProgram(program); });

		// 向 program 附加 vs
		glAttachShader(program, vs);
		if (auto e = glGetError()) {
			lastErrorNumber = __LINE__;
			lastErrorMessage = "LoadProgram error. glAttachShader vs fail, glGetError() = ";
			lastErrorMessage.append(std::to_string(glGetError()));
			return 0;
		}

		// 向 program 附加 ps
		glAttachShader(program, fs);
		if (auto e = glGetError()) {
			lastErrorNumber = __LINE__;
			lastErrorMessage = "LoadProgram error. glAttachShader fs fail, glGetError() = ";
			lastErrorMessage.append(std::to_string(glGetError()));
			return 0;
		}

		// 链接
		glLinkProgram(program);

		// 状态容器
		GLint r = GL_FALSE;

		// 查询链接是否成功
		glGetProgramiv(program, GL_LINK_STATUS, &r);

		// 失败
		if (!r) {
			// 查询日志信息文本长度
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &r);
			if (r) {
				// 复制错误文本
				lastErrorMessage.resize(r);
				glGetProgramInfoLog(program, r, nullptr, lastErrorMessage.data());
			}
			return 0;
		}

		sg.Cancel();
		return program;
	}

	// target: GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, or GL_UNIFORM_BUFFER. 
	// usage:  GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY. 
	inline GLuint LoadBuffer(GLenum const& target, void const* const& data, GLsizeiptr const& len, GLenum const& usage = GL_STATIC_DRAW) {
		GLuint vbo = 0;
		// 申请
		glGenBuffers(1, &vbo);
		if (!vbo) {
			lastErrorNumber = __LINE__;
			lastErrorMessage = "LoadVBO error. glGenBuffers fail, glGetError() = ";
			lastErrorMessage.append(std::to_string(glGetError()));
			return 0;
		}
		auto sgVbo = xx::MakeScopeGuard([&] { glDeleteVertexArrays(1, &vbo); });

		// 类型绑定
		glBindBuffer(target, vbo);
		if (auto e = glGetError()) {
			lastErrorNumber = __LINE__;
			lastErrorMessage = "LoadVBO error. glBindBuffer fail, glGetError() = ";
			lastErrorMessage.append(std::to_string(glGetError()));
			return 0;
		}

		// 数据填充
		glBufferData(target, len, data, usage);
		if (auto e = glGetError()) {
			lastErrorNumber = __LINE__;
			lastErrorMessage = "LoadVBO error. glBufferData fail, glGetError() = ";
			lastErrorMessage.append(std::to_string(glGetError()));
			return 0;
		}

		sgVbo.Cancel();
		return vbo;
	}

	inline GLuint LoadVertices(void const* const& data, GLsizeiptr const& len) {
		return LoadBuffer(GL_ARRAY_BUFFER, data, len);
	}
	inline GLuint LoadIndexes(void const* const& data, GLsizeiptr const& len) {
		return LoadBuffer(GL_ELEMENT_ARRAY_BUFFER, data, len);
	}
};


struct Vec3f
{
	GLfloat x, y, z;
};
struct Color4f
{
	GLfloat r, g, b, a;
};
struct Vec3fColor4f
{
	Vec3f vec3f;
	Color4f color4f;
};

struct ShaderBase {
	static void Release(GLuint const& n) {
		glDeleteShader(n);
	}
};
struct ProgramBase {
	static void Release(GLuint const& n) {
		glDeleteProgram(n);
	}
};
struct BufferBase {
	static void Release(GLuint const& n) {
		glDeleteBuffers(1, &n);
	}
	GLsizei len = 0;
};
template<typename ResBase>
struct ResHolder : ResBase {
	GLuint n = 0;
	XX_FORCEINLINE operator GLuint const&() const { return n; }

	ResHolder() = default;
	ResHolder(ResHolder const&) = delete;
	ResHolder& operator=(ResHolder const&) = delete;

	ResHolder(ResHolder&& o) : n(o.n) {
		o.n = 0;
	}
	ResHolder& operator=(ResHolder&& o) {
		std::swap(n, o.n);
	}
	XX_FORCEINLINE void operator=(GLuint const& n) {
		Reset();
		this->n = n;
	}
	XX_FORCEINLINE void Reset() {
		if (n) {
			this->ResBase::Release(n);
		}
	}
	~ResHolder() {
		Reset();
	}
};

using Shader = ResHolder<ShaderBase>;
using Program = ResHolder<ProgramBase>;
using Buffer = ResHolder<BufferBase>;

struct Game : Env<Game> {

	inline static const std::array<Vec3fColor4f, 3> vertices = {
		Vec3fColor4f{{-0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},  // v0 c0
		Vec3fColor4f{{-1.0f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},  // v1 c1
		Vec3fColor4f{{ 0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  // v2 c2
	};

	inline static const std::array<GLushort, 3> indices = {
		0, 1, 2
	};

	Shader vs, fs;
	Program ps;
	Buffer vb, ib;

	inline int GLInit() {
		vs = LoadVertexShader({ R"--(
#version 300 es
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;
out vec4 vColor;
void main()
{
   vColor = aColor;
   gl_Position = aPosition;
}
)--" });
		if (!vs) return __LINE__;

		fs = LoadFragmentShader({ R"--(
#version 300 es
precision mediump float;
in vec4 vColor;
out vec4 oFragColor;
void main()
{
   oFragColor = vColor;
}
)--" });
		if (!fs) return __LINE__;

		ps = LinkProgram(vs, fs);
		if (!ps) return __LINE__;

		vb = LoadVertices(vertices.data(), sizeof(vertices));
		if (!vb) return __LINE__;

		ib = LoadIndexes(indices.data(), sizeof(indices));
		if (!ib) return __LINE__;
		ib.len = sizeof(indices);

		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0, 0, 0, 1);

		return 0;
	}

	void Draw(Buffer const& vb, Buffer const& ib) {
		glUseProgram(ps);

		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, sizeof(Vec3f) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vec3fColor4f), (GLvoid*)offsetof(Vec3fColor4f, vec3f));
		glVertexAttribPointer(1, sizeof(Color4f) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vec3fColor4f), (GLvoid*)offsetof(Vec3fColor4f, color4f));

		glDrawElements(GL_TRIANGLES, ib.len, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	XX_FORCEINLINE void Update() {
		DrawFps();
		glViewport(0, 0, width, height);

		//glDepthMask(true);
		//glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDepthMask(false);

		Draw(vb, ib);
	}
};

int main() {
	Game g;
	if (int r = g.Run()) {
		std::cout << "g.Run() r = " << r << ", lastErrorNumber = " << g.lastErrorNumber << ", lastErrorMessage = " << g.lastErrorMessage << std::endl;
	}
}
