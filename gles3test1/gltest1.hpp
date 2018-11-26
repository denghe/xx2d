#pragma once

struct GLTest1
{
	inline static const char* vertex_shader_text = R"--(
uniform mat4 MVP;
attribute vec3 vCol;
attribute vec2 vPos;
varying vec3 color;
void main()
{
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    color = vCol;
}
)--";

	inline static const char* fragment_shader_text = R"--(
varying vec3 color;
void main()
{
    gl_FragColor = vec4(color, 1.0);
}
)--";


	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	struct Verticle
	{
		float x, y;
		float r, g, b;
	};
	Verticle vertices[3] =
	{
		{ -0.6f, -0.4f, 1.f, 0.f, 0.f },
		{  0.6f, -0.4f, 0.f, 1.f, 0.f },
		{   0.f,  0.6f, 0.f, 0.f, 1.f }
	};

	GLTest1()
	{
		// todo: ES 3.0

		glGenBuffers(1, &vertex_buffer);	// malloc
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);	// set data type: vertexs
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// memcpy + hint: 1 write, N read

		// load & compile vs
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
		glCompileShader(vertex_shader);

		// load & compile fs
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
		glCompileShader(fragment_shader);

		// combine & link
		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		// locate properties
		mvp_location = glGetUniformLocation(program, "MVP");
		vpos_location = glGetAttribLocation(program, "vPos");
		vcol_location = glGetAttribLocation(program, "vCol");

		glEnableVertexAttribArray(vpos_location);	// enable property
		glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,	sizeof(float) * 5, (void*)0);	// set data's len & offset

		glEnableVertexAttribArray(vcol_location);
		glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,	sizeof(float) * 5, (void*)(sizeof(float) * 2));
	}

	inline void Update(int const& width, int const& height, float time) noexcept
	{
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		// todo: 矩阵相关函数换用 gameplay3d 的
		mat4x4 m, p, mvp;
		var ratio = width / (float)height;
		mat4x4_identity(m);
		mat4x4_rotate_Z(m, m, time);
		mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		mat4x4_mul(mvp, p, m);

		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
};
