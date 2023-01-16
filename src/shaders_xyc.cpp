#include "pch.h"

void Shader_XyC::Init(ShaderManager* sm) {
	this->sm = sm;
	v = LoadGLVertexShader({ R"(#version 300 es
precision highp float;
uniform vec2 uCxy;	// center x y

in vec2 aPos;
in vec4 aColor;

out vec4 vColor;

void main() {
	gl_Position = vec4(aPos / uCxy, 0, 1);
	vColor = aColor;
})"sv });

	f = LoadGLFragmentShader({ R"(#version 300 es
precision highp float;

in vec4 vColor;

out vec4 oColor;

void main() {
	oColor = vColor;
})"sv });

	p = LinkGLProgram(v, f);

	uCxy = glGetUniformLocation(p, "uCxy");

	aPos = glGetAttribLocation(p, "aPos");
	aColor = glGetAttribLocation(p, "aColor");
	CheckGLError();

	glGenVertexArrays(1, &va.Ref());
	glBindVertexArray(va);
	glGenBuffers(1, (GLuint*)&vb);
	glGenBuffers(1, (GLuint*)&ib);

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYRGBA8), 0);
	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XYRGBA8), (GLvoid*)offsetof(XYRGBA8, r));
	glEnableVertexAttribArray(aColor);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CheckGLError();
}

void Shader_XyC::Begin() {
	glUseProgram(p);
	glUniform2f(uCxy, sm->eg->w / 2, sm->eg->h / 2);

	glBindVertexArray(va);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
}

void Shader_XyC::End() {
	if (indexsCount) {
		Commit();
	}
	// todo: cleanup buf & shader?
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// glBindVertexArray(0);
	// glUseProgram(0);
}


void Shader_XyC::Commit() {
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(XYRGBA8) * pointsCount, points.get(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indexsCount, indexs.get(), GL_DYNAMIC_DRAW);

	glDrawElements(GL_LINE_STRIP, indexsCount, GL_UNSIGNED_SHORT, 0);

	sm->drawLines += linesCount;
	sm->drawCall += 1;

	linesCount = 0;
	pointsCount = 0;
	indexsCount = 0;
}

XYRGBA8* Shader_XyC::DrawLineStrip(size_t const& pc) {
	assert(pc <= maxIndexNums);
	if (indexsCount + pc + 1 > maxIndexNums) {
		Commit();
	}
	auto rtv = &points[pointsCount];
	for (size_t i = pointsCount, e = pointsCount + pc; i < e; ++i) {
		indexs[indexsCount++] = i;
	}
	indexs[indexsCount++] = 65535;
	linesCount += pc - 1;
	pointsCount += pc;
	return rtv;
}

void Shader_XyC::DrawLineStrip(XYRGBA8* pointsBuf, size_t const& pointsCount) {
	memcpy(DrawLineStrip(pointsCount), pointsBuf, sizeof(XYRGBA8) * pointsCount);
}
