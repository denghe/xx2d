#pragma once

// gl es 3.0 test

// https://www.khronos.org/registry/OpenGL-Refpages/es3.0/

// 确保这些值为 0 以使 if 部分简写判断成立
#if GL_NO_ERROR != 0 && GL_FALSE != 0
#error
#endif

// 将 GL 错误转为枚举方便使用
enum class GLErrors : GLenum
{
	NoError = GL_NO_ERROR,
	InvalidEnum = GL_INVALID_ENUM,
	InvalidValue = GL_INVALID_VALUE,
	InvalidOperation = GL_INVALID_OPERATION,
	InvalidFrameBufferOperation = GL_INVALID_FRAMEBUFFER_OPERATION,
	OutOfMemory = GL_OUT_OF_MEMORY
};

#include <unordered_set>

struct GLShaderManager
{
	// 存储最后一次发生的错误的文本信息
	std::string lastErrorMessage;

	// 存储已经创建成功的 shader 资源id
	std::unordered_set<GLuint> shaders;

	// 存储已经创建成功的 program 资源id
	std::unordered_set<GLuint> programs;

	// 加载指定类型 shader
	// 返回 0 表示失败, 错误信息在 lastErrorMessage
	inline GLuint LoadShader(GLenum const& type, const GLchar* const& src, GLint const& len)
	{
		// 先判断下这个, 如果先申请在判断到错误, 还要 glDeleteShader 太麻烦.
		if (!src)
		{
			lastErrorMessage = "LoadShader error. arg: shaderSrc can't be nullptr";
			return 0;
		}

		// 申请 shader 上下文. 返回 0 表示失败, type 只能是 GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
		var shader = glCreateShader(type);
		if (!shader)
		{
			lastErrorMessage = "LoadShader error. arg: type's value must be GL_VERTEX_SHADER or GL_FRAGMENT_SHADER";
			return 0;
		}

		// args: target shader, array nums, char* array, len array
		// 意思是可以支持由多个代码片段组合出来的字串, 避免拷贝它们
		glShaderSource(shader, 1, &src, len ? &len : nullptr);

		// 编译. 状态查询要用 glGetShaderiv
		glCompileShader(shader);

		// 通用容器变量
		GLint v = 0;

		// 查询是否编译成功
		glGetShaderiv(shader, GL_COMPILE_STATUS, &v);			// GL_TRUE, GL_FALSE

		// 失败
		if (!v)
		{
			// 查询日志信息文本长度
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &v);		// log message's len
			if (v)
			{
				// 分配空间并复制
				lastErrorMessage.resize(v);
				glGetShaderInfoLog(shader, v, nullptr, lastErrorMessage.data());
			}
			// 删掉 shader 上下文
			glDeleteShader(shader);
			return 0;
		}

		// 成功. 放入容器并返回 id
		shaders.emplace(shader);
		return shader;
	}

	// load shader helpers

	template<size_t len>
	inline GLuint LoadShader(GLenum const& type, char const(&src)[len])
	{
		return LoadShader(type, src, len);
	}

	template<size_t len>
	inline GLuint LoadVertexShader(char const(&src)[len])
	{
		return LoadShader(GL_VERTEX_SHADER, src, len);
	}
	inline GLuint LoadVertexShader(const GLchar* const& src, GLint const& len)
	{
		return LoadShader(GL_VERTEX_SHADER, src, len);
	}

	template<size_t len>
	inline GLuint LoadFragmentShader(char const(&src)[len])
	{
		return LoadShader(GL_FRAGMENT_SHADER, src, len);
	}
	inline GLuint LoadFragmentShader(const GLchar* const& src, GLint const& len)
	{
		return LoadShader(GL_FRAGMENT_SHADER, src, len);
	}

	// 用 vs fs 链接出 program
	// 返回 0 表示失败, 错误信息在 lastErrorMessage
	inline GLuint LinkProgram(GLuint vs, GLuint fs)
	{
		// 确保 vs, fs 已存在
		if (shaders.find(vs) == shaders.cend())
		{
			lastErrorMessage = "LoadProgram error. can't find vs = ";
			lastErrorMessage.append(std::to_string(vs));
			return 0;
		}
		if (shaders.find(fs) == shaders.cend())
		{
			lastErrorMessage = "LoadProgram error. can't find fs = ";
			lastErrorMessage.append(std::to_string(fs));
			return 0;
		}

		// 创建一个程序对象. 返回 0 表示失败
		var program = glCreateProgram();
		if (!program)
		{
			lastErrorMessage = "LoadProgram error. glCreateProgram fail, glGetError() = ";
			lastErrorMessage.append(std::to_string(glGetError()));
			return 0;
		}
		xx::ScopeGuard sgProg([&] { glDeleteProgram(program); });

		// 向 program 附加 vs
		glAttachShader(program, vs);
		if (var e = glGetError())
		{
			lastErrorMessage = "LoadProgram error. glAttachShader vs fail, glGetError() = ";
			lastErrorMessage.append(std::to_string(glGetError()));
			return 0;
		}

		// 向 program 附加 ps
		glAttachShader(program, fs);
		if (var e = glGetError())
		{
			lastErrorMessage = "LoadProgram error. glAttachShader fs fail, glGetError() = ";
			lastErrorMessage.append(std::to_string(glGetError()));
			return 0;
		}

		// 链接. 状态查询要用 glGetShaderiv
		glLinkProgram(program);

		// 通用容器变量
		GLint v = 0;

		// 查询链接是否成功
		glGetProgramiv(program, GL_LINK_STATUS, &v);			// GL_TRUE, GL_FALSE

		// 失败
		if (!v)
		{
			// 查询日志信息文本长度
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &v);	// log message's len
			if (v)
			{
				// 分配空间并复制
				lastErrorMessage.resize(v);
				glGetProgramInfoLog(program, v, nullptr, lastErrorMessage.data());
			}

			glDeleteProgram(program);
			return 0;
		}

		// 成功. 放入容器. 撤销对 program 的自动回收函数 并返回 id
		programs.emplace(program);
		sgProg.Cancel();
		return program;
	}

	// todo: glDetachShader 用于拆开一个 program. 可以替换 vs fs 再次链接

	~GLShaderManager()
	{
		for (var p : programs)
		{
			glDeleteProgram(p);
		}
		for (var s : programs)
		{
			glDeleteShader(s);
		}
	}
};

struct GLTest2
{
	inline static var vsSrc = R"--(
#version 300 es
layout(location = 0) in vec4 vPosition;
void main()
{
   gl_Position = vPosition;
}
)--";

	inline static var fsSrc = R"--(
#version 300 es
precision mediump float;
out vec4 fragColor;
void main()
{
   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );
}
)--";

	inline static GLfloat verts[] =
	{
		0.0f,   0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f,
		0.5f,  -0.5f,  0.0f
	};

	// shader 管理器
	GLShaderManager sm;

	// 存储最后一次发生的错误的文本信息
	std::string lastErrorMessage;

	// 存放编译后的编号以便 Update 中使用
	GLuint program = 0;

	GLTest2()
	{
		var vs = sm.LoadShader(GL_VERTEX_SHADER, vsSrc);
		assert(vs);
		var fs = sm.LoadShader(GL_FRAGMENT_SHADER, fsSrc);
		assert(fs);
		program = sm.LinkProgram(vs, fs);
		assert(program);

		// 设置清屏颜色
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	inline void Update(int const& width, int const& height, float time) noexcept
	{
		// 防止 glViewport 出错
		assert(width >= 0 && height >= 0);

		// 设置 gl 在窗口中的显示区域(  x + w/2, y + h/2  是 0, 0 点, 上下左右最大坐标值为 1 )
		glViewport(0, 0, width, height);

		// 使用 glClearColor 设的值来清屏
		glClear(GL_COLOR_BUFFER_BIT);

		// 传0 是未定义行为
		assert(program);

		// 指定 program 到当前上下文
		glUseProgram(program);
		if (var e = glGetError())
		{
			lastErrorMessage.append("glUseProgram glGetError() = ");
			lastErrorMessage.append(std::to_string(glGetError()));
			return;
		}

		// todo: 根据变量名来得到属性下标, 取代下面用到的 0

		// 改上下文中的 program 的 通用顶点属性数
		// 参数表: GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,	const GLvoid * pointer
		// index: 0, 与 shader 代码里的 layout(location = 0) 相对应
		// size: 3, 与 verts 数组相对应. 虽然 in vec4 vPosition 长度是 4, 但是这样可以填充 vPosition 的前 3 个属性: x, y, z
		// type: GL_FLOAT, 表明 verts 数组 的成员的数据类型为 float
		// normalized: GL_FALSE, specifies whether fixed-point data values should be normalized (GL_TRUE) or converted directly as fixed-point values (GL_FALSE) when they are accessed. This parameter is ignored if type is GL_FIXED.
		// stride: 0, 数据跨距为 0, 与 verts 数组里全是顶点数据的情况相符. 
		// pointer: verts, 告之数据指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, verts);
		if (var e = glGetError())
		{
			lastErrorMessage.append("glVertexAttribPointer glGetError() = ");
			lastErrorMessage.append(std::to_string(glGetError()));
			return;
		}

		// 启用顶点属性数组
		glEnableVertexAttribArray(0);
		if (var e = glGetError())
		{
			lastErrorMessage.append("glEnableVertexAttribArray glGetError() = ");
			lastErrorMessage.append(std::to_string(glGetError()));
			return;
		}

		// 使用数组数据绘制图形. 
		// 参数表: GLenum mode, GLint first, GLsizei count
		// mode: GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES
		// first: 指定数组中的数据的起始下标. 实际数据偏移量为 first * ((顶点数据个数 + 跨距个数) * sizeof(数据类型))
		// count: 指定要绘制多少个顶点
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	}
};
