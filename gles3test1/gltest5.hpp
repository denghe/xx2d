#pragma once

#include "glmanager.hpp"
#include "esMatrix.h"

inline static std::array<GLfloat, 24 * 3> vertices = {
	   -0.5f, -0.5f, -0.5f,
	   -0.5f, -0.5f,  0.5f,
	   0.5f, -0.5f,  0.5f,
	   0.5f, -0.5f, -0.5f,
	   -0.5f,  0.5f, -0.5f,
	   -0.5f,  0.5f,  0.5f,
	   0.5f,  0.5f,  0.5f,
	   0.5f,  0.5f, -0.5f,
	   -0.5f, -0.5f, -0.5f,
	   -0.5f,  0.5f, -0.5f,
	   0.5f,  0.5f, -0.5f,
	   0.5f, -0.5f, -0.5f,
	   -0.5f, -0.5f, 0.5f,
	   -0.5f,  0.5f, 0.5f,
	   0.5f,  0.5f, 0.5f,
	   0.5f, -0.5f, 0.5f,
	   -0.5f, -0.5f, -0.5f,
	   -0.5f, -0.5f,  0.5f,
	   -0.5f,  0.5f,  0.5f,
	   -0.5f,  0.5f, -0.5f,
	   0.5f, -0.5f, -0.5f,
	   0.5f, -0.5f,  0.5f,
	   0.5f,  0.5f,  0.5f,
	   0.5f,  0.5f, -0.5f,
};

inline static std::array<GLushort, 12 * 3> indices = {
	   0, 2, 1,
	   0, 3, 2,
	   4, 5, 6,
	   4, 6, 7,
	   8, 9, 10,
	   8, 10, 11,
	   12, 15, 14,
	   12, 14, 13,
	   16, 17, 18,
	   16, 18, 19,
	   20, 23, 22,
	   20, 22, 21
};

inline static int iPosition = 0;
inline static int uMvpMatrix = 0;
inline static int uColor = 0;
inline static auto vsSrc = R"--(
#version 300 es
in vec4 iPosition;
uniform vec4 uColor;
uniform mat4 uMvpMatrix;
out vec4 vColor;
void main() {
	gl_Position = uMvpMatrix * iPosition;
	vColor = uColor;
}
)--";

inline static auto fsSrc = R"--(
#version 300 es
precision mediump float;
in vec4 vColor;
out vec4 oColor;
void main() {
   oColor = vColor;
}
)--";

struct GLTest5
{
	GLManager sm;
	std::string lastErrorMessage;
	GLuint vs, fs, ps, vb, ib;
	float angle = 0;

	GLTest5()
	{
		vs = sm.LoadShader(GL_VERTEX_SHADER, vsSrc);
		assert(vs);
		fs = sm.LoadShader(GL_FRAGMENT_SHADER, fsSrc);
		assert(fs);
		ps = sm.LinkProgram(vs, fs);
		assert(ps);

		iPosition = glGetAttribLocation(ps, "iPosition");
		uMvpMatrix = glGetUniformLocation(ps, "uMvpMatrix");
		uColor = glGetUniformLocation(ps, "uColor");

		for (auto& v : vertices) {
			v *= 0.1f;
		}

		vb = sm.LoadVBO(GL_ARRAY_BUFFER, vertices.data(), sizeof(vertices));
		assert(vb);
		ib = sm.LoadVBO(GL_ELEMENT_ARRAY_BUFFER, indices.data(), sizeof(indices));
		assert(ib);

		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	inline void Update(int const& width, int const& height, float deltaTime) noexcept {
		ESMatrix matrixBuf;
		{
			ESMatrix perspective;
			auto aspect = (GLfloat)width / (GLfloat)height;
			esMatrixLoadIdentity(&perspective);
			esOrtho(&perspective, -aspect, aspect, -1, 1, -10, 10);

			ESMatrix modelview;
			float translateX = -1.0f;
			float translateY = -1.0f;

			esMatrixLoadIdentity(&modelview);
			esTranslate(&modelview, translateX, translateY, -2.0f);
			angle += (deltaTime * 4.0f);
			if (angle >= 360.0f) {
				angle -= 360.0f;
			}
			esRotate(&modelview, angle, 1.0, 0.0, 1.0);
			esMatrixMultiply(&matrixBuf, &modelview, &perspective);
		}


		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ps);

		glUniformMatrix4fv(uMvpMatrix, 1, GL_FALSE, (float*)&matrixBuf);
		glUniform4f(uColor, 1, 0.5, 0.5, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glVertexAttribPointer(iPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void*)0);
		glEnableVertexAttribArray(iPosition);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, 0);
	}
};
