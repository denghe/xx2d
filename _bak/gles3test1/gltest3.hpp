#pragma once

#include "glmanager.hpp"

#pragma pack(push, 1)
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
#pragma pack(pop)

struct GLTest3
{
	inline static var vsSrc = R"--(
#version 300 es
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;
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

	inline static const std::array<Vec3fColor4f, 3> vertices =
	{
		Vec3fColor4f{{-0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},  // v0 c0
		Vec3fColor4f{{-1.0f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},  // v1 c1
		Vec3fColor4f{{ 0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  // v2 c2
	};

	// Index buffer data
	inline static const std::array<GLushort, 3> indices =
	{
		0, 1, 2
	};

	GLManager gm;
	std::string lastErrorMessage;
	GLuint program = 0;
	GLuint vertsId = 0, idxsId = 0;

	void DrawPrimitiveWithVBOs()
	{
		GLuint offset = 0;

		assert(vertsId && idxsId);
		glBindBuffer(GL_ARRAY_BUFFER, vertsId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxsId);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, sizeof(Vec3f) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vec3fColor4f), 0);
		glVertexAttribPointer(1, sizeof(Color4f) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vec3fColor4f), (void*)sizeof(Vec3f));

		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	GLTest3()
	{
		var vs = gm.LoadShader(GL_VERTEX_SHADER, vsSrc);
		assert(vs);
		var fs = gm.LoadShader(GL_FRAGMENT_SHADER, fsSrc);
		assert(fs);
		program = gm.LinkProgram(vs, fs);
		assert(program);
		vertsId = gm.LoadVBO(GL_ARRAY_BUFFER, vertices.data(), sizeof(vertices));
		assert(vertsId);
		idxsId = gm.LoadVBO(GL_ELEMENT_ARRAY_BUFFER, indices.data(), sizeof(indices));
		assert(idxsId);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	inline void Update(int const& width, int const& height, float time) noexcept
	{
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);
		DrawPrimitiveWithVBOs();
	}
};
