#pragma once
#include <xx_helpers.h>
#include <xx_file.h>
#include "glew/glew.h"
#include "glfw/glfw3.h"
#include "esMatrix.h"

namespace xx::es {

	enum class GLResTypes {
		shader, program, vertexss, buffer, texture
	};

	template<GLResTypes T, typename UD = GLsizei>
	struct GLRes {
		GLuint handle{};
		UD ud{};

		operator GLuint const& () const { return handle; }

		GLRes() = default;
		GLRes(GLRes const&) = delete;
		GLRes& operator=(GLRes const&) = delete;

		GLRes(GLuint const& handle) : handle(handle) {}
		GLRes(GLRes&& o) {
			this->operator=(std::move(o));
		}
		GLRes& operator=(GLRes&& o) {
			std::swap(handle, o.handle);
			std::swap(ud, o.ud);
			return *this;
		}

		bool operator==(GLRes const& o) { return handle == o.handle; }
		bool operator!=(GLRes const& o) { return handle != o.handle; }
		bool operator==(GLuint const& o) { return handle == o; }
		bool operator!=(GLuint const& o) { return handle != o; }

		void operator=(GLuint const& h) {
			Reset();
			handle = h;
		}
		void Reset() {
			if (handle) {
				if constexpr (T == GLResTypes::shader) { glDeleteShader(handle); }
				if constexpr (T == GLResTypes::program) { glDeleteProgram(handle); }
				if constexpr (T == GLResTypes::vertexss) { glDeleteVertexArrays(1, &handle); }
				if constexpr (T == GLResTypes::buffer) { glDeleteBuffers(1, &handle); }
				if constexpr (T == GLResTypes::texture) { glDeleteTextures(1, &handle); }
				handle = 0;
			}
		}
		~GLRes() {
			Reset();
		}
	};

	using Shader = GLRes<GLResTypes::shader>;
	using Program = GLRes<GLResTypes::program>;
	using VertexArrays = GLRes<GLResTypes::vertexss>;
	using Buffer = GLRes<GLResTypes::buffer>;
	using Texture = GLRes<GLResTypes::texture, std::pair<GLsizei, GLsizei>>;

	union Color4b {
		struct {
			uint8_t r, g, b, a;
		};
		uint32_t data = 0;
	};




	template<typename T>
	struct Context {
		inline static Context* instance = nullptr;

		int width = 1280;
		int height = 720;
		int vsync = 1;	// 0: 0ff
		bool fullScreen = false;
		std::string title = "gles3test2 fps: ";
		std::filesystem::path rootPath = std::filesystem::current_path() / "res";

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

		void onGLFWError(int num, const char* msg) {
			lastErrorNumber = num;
			lastErrorMessage = msg;
		}
		void onMouseButtonCallback(int button, int action, int modify) {
			//std::cout << button << " " << action << " " << modify << std::endl;
		}
		void onCursorPosCallback(double x, double y) {
			//std::cout << x << " " << y << std::endl;
		}
		void onScrollCallback(double x, double y) {
			//std::cout << x << " " << y << std::endl;
		}
		void onKeyCallback(int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(wnd, GLFW_TRUE);
			}
		}
		void onCharCallback(unsigned int character) {}
		void onWindowPosCallback(int x, int y) {}
		void onFramebufferSizeCallback(int w, int h) {
			width = w;
			height = h;
		}
		void onWindowSizeCallback(int w, int h) {
			width = w;
			height = h;
		}
		void onWindowIconifyCallback(int iconified) {}
		void onWindowFocusCallback(int focused) {}

		Context() {
			instance = this;
		}
		virtual ~Context() {
			instance = nullptr;
		}

		int Run() {
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


		void ShowFpsInTitle() {
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
		Shader LoadShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_) {
			// 前置检查
			if (!codes_.size() || !(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER)) {
				lastErrorNumber = __LINE__;
				lastErrorMessage = "LoadShader if (!codes.size() || !(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER))";
				return 0;
			}

			// 申请 shader 上下文. 返回 0 表示失败, 参数：GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER
			auto&& shader = glCreateShader(type);
			if (!shader) { CheckGLError(__LINE__); return 0; }
			auto&& sg = MakeScopeGuard([&] { glDeleteShader(shader); });

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

		Shader LoadVertexShader(std::initializer_list<std::string_view>&& codes_) {
			return LoadShader(GL_VERTEX_SHADER, std::move(codes_));
		}
		Shader LoadFragmentShader(std::initializer_list<std::string_view>&& codes_) {
			return LoadShader(GL_FRAGMENT_SHADER, std::move(codes_));
		}

		// 用 vs fs 链接出 program. 返回 0 表示失败
		Program LinkProgram(GLuint const& vs, GLuint const& fs) {
			// 创建一个 program. 返回 0 表示失败
			auto program = glCreateProgram();
			if (!program) { CheckGLError(__LINE__); return 0; }
			auto sg = MakeScopeGuard([&] { glDeleteProgram(program); });

			// 向 program 附加 vs
			glAttachShader(program, vs);
			if (auto e = CheckGLError(__LINE__)) return 0;

			// 向 program 附加 ps
			glAttachShader(program, fs);
			if (auto e = CheckGLError(__LINE__)) return 0;

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

		//// target: GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, or GL_UNIFORM_BUFFER. 
		//// usage:  GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY. 
		//Buffer LoadBuffer(GLenum const& target, void const* const& data, GLsizeiptr const& len, GLenum const& usage = GL_STATIC_DRAW) {
		//	GLuint vbo = 0;
		//	// 申请
		//	glGenBuffers(1, &vbo);
		//	if (!vbo) { CheckGLError(__LINE__); return 0; }
		//	auto sgVbo = MakeScopeGuard([&] { glDeleteVertexArrays(1, &vbo); });

		//	// 类型绑定
		//	glBindBuffer(target, vbo);
		//	if (auto e = CheckGLError(__LINE__)) return 0;

		//	// 数据填充
		//	glBufferData(target, len, data, usage);
		//	if (auto e = CheckGLError(__LINE__)) return 0;

		//	glBindBuffer(target, 0);
		//	sgVbo.Cancel();
		//	return vbo;
		//}

		//Buffer LoadVertices(void const* const& data, GLsizeiptr const& len, GLenum const& usage = GL_STATIC_DRAW) {
		//	return LoadBuffer(GL_ARRAY_BUFFER, data, len, usage);
		//}
		//Buffer LoadIndexes(void const* const& data, GLsizeiptr const& len, GLenum const& usage = GL_STATIC_DRAW) {
		//	return LoadBuffer(GL_ELEMENT_ARRAY_BUFFER, data, len, usage);
		//}


		// 加载 etc2 压缩纹理数据. 返回 0 表示出错
		Texture LoadEtc2TextureData(void* const& data, GLsizei const& len, GLsizei const& width, GLsizei const& height, bool const& hasAlpha = true) {
			GLuint t = 0;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glGenTextures(1, &t);
			if (auto e = CheckGLError(__LINE__)) return 0;
			auto sg = MakeScopeGuard([&] { glDeleteTextures(1, &t); });
			glBindTexture(GL_TEXTURE_2D, t);
			if (auto e = CheckGLError(__LINE__)) return 0;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST/*GL_LINEAR*/);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST/*GL_LINEAR*/);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, hasAlpha ? GL_COMPRESSED_RGBA8_ETC2_EAC : GL_COMPRESSED_RGB8_ETC2, width, height, 0, len, data);
			if (auto e = CheckGLError(__LINE__)) return 0;

			sg.Cancel();
			Texture rtv = t;
			rtv.ud.first = width;
			rtv.ud.second = height;
			return rtv;
		}

		// 加载 文件头部以 "PKM 20" 或 "«KTX 11»" 打头的压缩纹理数据文件
		// 支持加载 zip 压缩后的 pkm 或 ktx 文件。头部为 50 4B 03 04 "PK.."，里面只可以有 1 个不带路径的数据文件  ( todo )
		Texture LoadEtc2TextureFile(std::filesystem::path const& fp) {
			xx::Data d;
			if (int r = xx::ReadAllBytes(fp, d)) {
				lastErrorNumber = __LINE__;
				lastErrorMessage = std::string("read file error. rtv = ") + std::to_string(r) + ", path = " + fp.string();
				return 0;
			}
			if (d.len >= 4 && memcmp("PK\x03\x04", d.buf, 4) == 0) {
				// todo: decompress, fill data to d
			}
			if (d.len >= 6 && memcmp("PKM 20", d.buf, 6) == 0 && d.len >= 16) {
				auto p = (uint8_t*)d.buf;
				uint16_t format = (p[6] << 8) | p[7];				// 格式值:  1 ETC2_RGB_NO_MIPMAPS, 3 ETC2_RGBA_NO_MIPMAPS
				uint16_t encodedWidth = (p[8] << 8) | p[9];			// 4 字节对齐 宽
				uint16_t encodedHeight = (p[10] << 8) | p[11];		// 4 字节对齐 高
				uint16_t width = (p[12] << 8) | p[13];				// 宽
				uint16_t height = (p[14] << 8) | p[15];				// 高
				if ((format == 1 || format == 3)
					&& width > 0 && height > 0
					&& encodedWidth >= width && encodedWidth - width < 4
					&& encodedHeight >= height && encodedHeight - height < 4
					&& d.len == 16 + encodedWidth * encodedHeight
					) {
					//return LoadEtc2TextureData(d.buf + 16, (GLsizei)(d.len - 16), encodedWidth, encodedHeight, false/*format == 3*/);
					return LoadEtc2TextureData(d.buf + 16, (GLsizei)(d.len - 16), width, height, format == 3);
				}
			}
			else if (d.len >= 8 && memcmp("\xABKTX 11\xBB", d.buf, 8) == 0) {
				// todo
			}
			lastErrorNumber = __LINE__;
			lastErrorMessage = "bad file format type";
			return 0;
		}
	};
}
