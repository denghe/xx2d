#pragma once
#include "xx_ptr.h"
#include "xx_chrono.h"
#include "glew/glew.h"
#include "glfw/glfw3.h"
#include "esMatrix.h"

namespace xx::es {
	template<typename T>
	struct Context {
		inline static Context* instance = nullptr;

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

		float w = 1280;
		float h = 720;
		double beginTime = 0;
		double lastTime = 0;
		float elapsedSeconds = 0;
		float delta = 0;
		uint64_t numFrames = 0;
		double fpsTimer = 0;

		inline void onGLFWError(int num, const char* msg) {
			lastErrorNumber = num;
			lastErrorMessage = msg;
		}
		inline void onMouseButtonCallback(int button, int action, int modify) {
			//std::cout << button << " " << action << " " << modify << std::endl;
		}
		inline void onCursorPosCallback(double x, double y) {
			//std::cout << x << " " << y << std::endl;
		}
		inline void onScrollCallback(double x, double y) {
			//std::cout << x << " " << y << std::endl;
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
		inline void onWindowSizeCallback(int w, int h) {
			width = w;
			height = h;
		}
		inline void onWindowIconifyCallback(int iconified) {}
		inline void onWindowFocusCallback(int focused) {}

		Context() {
			instance = this;
		}
		virtual ~Context() {
			instance = nullptr;
		}

		inline int Run() {
			/************************************************************************************************/
			// 初始化窗口容器

			glfwSetErrorCallback([](auto n, auto m) { instance->onGLFWError(n, m); });
			if (glfwInit() == GLFW_FALSE) return __LINE__;
			auto sg = MakeScopeGuard([] {
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
			auto sg2 = MakeScopeGuard([this] {
				glfwDestroyWindow(wnd);
				wnd = nullptr;
				});

			glfwMakeContextCurrent(wnd);
			glfwSwapInterval(vsync);
			glfwGetWindowSize(wnd, &width, &height);
			w = (float)width;
			h = (float)height;

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

			beginTime = lastTime = NowSteadyEpochSeconds();
			fpsTimer = lastTime + 0.5;

			while (!glfwWindowShouldClose(wnd)) {
				++numFrames;
				delta = (float)NowSteadyEpochSeconds(lastTime);
				elapsedSeconds = (float)(lastTime - beginTime);

				((T*)this)->Update();
				((T*)this)->Draw();

				glfwSwapBuffers(wnd);

				glfwPollEvents();
			}

			return 0;
		}


		inline void ShowFpsInTitle() {
			if (lastTime > fpsTimer) {
				fpsTimer = lastTime + 0.5;
				glfwSetWindowTitle(wnd, (title + std::to_string((double)numFrames / (lastTime - beginTime))).c_str());
			}
		}


		int CheckGLError(int n) {
			if (auto e = glGetError()) {
				lastErrorNumber = n;
				lastErrorMessage = "glGetError() = ";
				lastErrorMessage.append(std::to_string(e));
				return e;
			}
			return 0;
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
			auto&& sg = MakeScopeGuard([&] {
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
			auto sg = MakeScopeGuard([&] { glDeleteProgram(program); });

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
			auto sgVbo = MakeScopeGuard([&] { glDeleteVertexArrays(1, &vbo); });

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

			glBindBuffer(target, 0);
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


	struct ShaderBase {
		static void Release(GLuint const& handle) {
			glDeleteShader(handle);
		}
	};
	struct ProgramBase {
		static void Release(GLuint const& handle) {
			glDeleteProgram(handle);
		}
	};
	struct BufferBase {
		static void Release(GLuint const& handle) {
			glDeleteBuffers(1, &handle);
		}
		GLsizei len = 0;
	};
	template<typename ResBase>
	struct ResHolder : ResBase {
		GLuint handle = 0;
		XX_FORCEINLINE operator GLuint const& () const { return handle; }

		ResHolder() = default;
		ResHolder(ResHolder const&) = delete;
		ResHolder& operator=(ResHolder const&) = delete;

		ResHolder(ResHolder&& o) : handle(o.handle) {
			o.handle = 0;
		}
		ResHolder& operator=(ResHolder&& o) {
			std::swap(handle, o.handle);
		}
		XX_FORCEINLINE void operator=(GLuint const& handle) {
			Reset();
			this->handle = handle;
		}
		XX_FORCEINLINE void Reset() {
			if (handle) {
				this->ResBase::Release(handle);
			}
		}
		~ResHolder() {
			Reset();
		}
	};

	using Shader = ResHolder<ShaderBase>;
	using Program = ResHolder<ProgramBase>;
	using Buffer = ResHolder<BufferBase>;

	union Color4b {
		struct {
			uint8_t r, g, b, a;
		};
		uint32_t data = 0;
	};

}
