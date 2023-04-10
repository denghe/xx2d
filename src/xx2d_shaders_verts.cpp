#include "xx2d.h"

namespace xx {

	void Shader_Verts::Init(ShaderManager* sm) {
		this->sm = sm;
		v = LoadGLVertexShader({ R"(#version 300 es
precision highp float;
uniform vec2 uCxy;	// screen center coordinate

in vec2 aPos;
in vec4 aColor;

out vec4 vColor;

void main() {
	gl_Position = vec4(aPos * uCxy, 0, 1);
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

	void Shader_Verts::Begin() {
		if (sm->cursor != index) {
			// here can check shader type for combine batch
			sm->shaders[sm->cursor]->End();
			sm->cursor = index;
		}

		engine.GLEnableBlend();

		glUseProgram(p);
		glUniform2f(uCxy, 2 / engine.w, 2 / engine.h);

		glBindVertexArray(va);
	}

	void Shader_Verts::End() {
		if (indexsCount) {
			Commit();
		}
	}

	void Shader_Verts::Commit() {
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(XYRGBA8) * vertsCount, verts.get(), GL_STREAM_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indexsCount, indexs.get(), GL_STREAM_DRAW);

		glDrawElements(GL_TRIANGLES, indexsCount, GL_UNSIGNED_SHORT, (GLvoid*)0);
		CheckGLError();

		sm->drawVerts += indexsCount;
		sm->drawCall += 1;

		vertsCount = 0;
		indexsCount = 0;
	}

	std::tuple<size_t, XYRGBA8*, uint16_t*> Shader_Verts::Draw(size_t const& numVerts, size_t const& numIndexs) {
		assert(numVerts <= maxVertNums);
		assert(numIndexs <= maxIndexNums);
		if (vertsCount + numVerts > maxVertNums || indexsCount + numIndexs > maxIndexNums) {
			Commit();
		}
		std::tuple<size_t, XYRGBA8*, uint16_t*> r{ vertsCount, &verts[vertsCount], &indexs[indexsCount] };
		vertsCount += numVerts;
		indexsCount += numIndexs;
		return r;
	}
}
