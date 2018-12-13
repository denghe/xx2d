#pragma once

#include "glmanager.hpp"

struct GLTest2
{
	inline static var vsSrc = R"--(
#version 300 es
layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aPosition;
out vec4 vColor;
void main()
{
   vColor = aColor;
   gl_Position = aPosition;
}
)--";

	inline static var fsSrc = R"--(
#version 300 es
precision mediump float;
in vec4 vColor;
out vec4 oFragColor;
void main()
{
   oFragColor = vColor;
}
)--";

	inline static std::array<GLfloat, 4> color = 
	{
		1.0f, 0.0f, 0.0f, 1.0f
	};
	inline static std::array<GLfloat, 9> verts =
	{
		0.0f,   0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f,
		0.5f,  -0.5f,  0.0f
	};

	// shader 管理器
	GLManager gm;

	// 存储最后一次发生的错误的文本信息
	std::string lastErrorMessage;

	// 存放编译后的编号以便 Update 中使用
	GLuint program = 0;

	GLTest2()
	{
		var vs = gm.LoadShader(GL_VERTEX_SHADER, vsSrc);
		assert(vs);
		var fs = gm.LoadShader(GL_FRAGMENT_SHADER, fsSrc);
		assert(fs);
		program = gm.LinkProgram(vs, fs);
		assert(program);

		// 设置清屏颜色
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

		// 用数组内容填充属性 location = 0 处长度为 4 的 float 值
		// 参数表: GLuint index, GLfloat* v
		// index: 与 shader 代码里的 layout(location 值 相对应
		// v: 传入数据所在数组
		// 出错可能: index 超范围. 这里写死的故不需要判断了
		glVertexAttrib4fv(0, color.data());
		

		// 改上下文中的 program 的 通用顶点属性数
		// 参数表: GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,	const GLvoid * pointer
		// index: 与 shader 代码里的 layout(location 值 相对应
		// size: 与 verts 数组相对应. 因为属性是 vec4, 容器长度是 4, 但数据长度是 3, 故填充为: x, y, z, 1.0f
		// type: 属性基本元素数据类型
		// normalized: false: 直接转为 float. true: float 类的保持不变, 符号整型映射到 -1 ~ 1, 无符整型映射到 0 ~ 1 区间
		// stride: 0, 数据跨距为 0, 与 verts 数组里全是顶点数据的情况相符. 
		// pointer: verts, 告之数据指针
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, verts.data());
		if (var e = glGetError())
		{
			lastErrorMessage.append("glVertexAttribPointer glGetError() = ");
			lastErrorMessage.append(std::to_string(glGetError()));
			return;
		}

		// 启用顶点属性数组
		// 出错可能: index 超范围. 这里写死的故不需要判断了
		glEnableVertexAttribArray(1);

		// 使用数组数据绘制图形. 
		// 参数表: GLenum mode, GLint first, GLsizei count
		// mode: GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES
		// first: 指定数组中的数据的起始下标. 实际数据偏移量为 first * ((顶点数据个数 + 跨距个数) * sizeof(数据类型))
		// count: 指定要绘制多少个顶点
		glDrawArrays(GL_TRIANGLES, 0, 3);
		if (var e = glGetError())
		{
			lastErrorMessage.append("glDrawArrays glGetError() = ");
			lastErrorMessage.append(std::to_string(glGetError()));
			return;
		}
	}
};
