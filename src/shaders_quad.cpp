#include "pch.h"

namespace xx {

	void Shader_Quad::Init(ShaderManager* sm) {
		this->sm = sm;
		v = LoadGLVertexShader({ R"(#version 300 es
precision highp float;
uniform vec2 uCxy;	// screen center coordinate

in vec2 aPos;
in vec2 aTexCoord;
in vec4 aColor;

out vec4 vColor;
out vec2 vTexCoord;

void main() {
	gl_Position = vec4(aPos / uCxy, 0, 1);
	vTexCoord = aTexCoord;
	vColor = aColor;
})"sv });

		f = LoadGLFragmentShader({ R"(#version 300 es
precision highp float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord / vec2(textureSize(uTex0, 0)));
})"sv });

		p = LinkGLProgram(v, f);

		uCxy = glGetUniformLocation(p, "uCxy");
		uTex0 = glGetUniformLocation(p, "uTex0");

		aPos = glGetAttribLocation(p, "aPos");
		aTexCoord = glGetAttribLocation(p, "aTexCoord");
		aColor = glGetAttribLocation(p, "aColor");
		CheckGLError();

		glGenVertexArrays(1, &va.Ref());
		glBindVertexArray(va);
		glGenBuffers(1, (GLuint*)&vb);
		glGenBuffers(1, (GLuint*)&ib);

		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYUVRGBA8), 0);
		glEnableVertexAttribArray(aPos);
		glVertexAttribPointer(aTexCoord, 2, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, u));
		glEnableVertexAttribArray(aTexCoord);
		glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, r));
		glEnableVertexAttribArray(aColor);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		auto idxs = std::make_unique<GLushort[]>(maxIndexNums);
		for (size_t i = 0; i < maxVertNums / 4; i++) {
			auto p = idxs.get() + i * 6;
			auto v = i * 4;
			p[0] = uint16_t(v + 0);
			p[1] = uint16_t(v + 1);
			p[2] = uint16_t(v + 2);
			p[3] = uint16_t(v + 0);
			p[4] = uint16_t(v + 2);
			p[5] = uint16_t(v + 3);
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * maxIndexNums, idxs.get(), GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CheckGLError();
	}

	void Shader_Quad::Begin() {
		if (sm->cursor != index) {
			// here can check shader type for combine batch
			sm->shaders[sm->cursor]->End();
			sm->cursor = index;
		}

		glUseProgram(p);
		glUniform1i(uTex0, 0);
		glUniform2f(uCxy, engine.w / 2, engine.h / 2);

		glBindVertexArray(va);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	}

	void Shader_Quad::End() {
		if (quadVertsCount) {
			Commit();
		}
		// todo: cleanup buf & shader?
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);
		// glUseProgram(0);
	}

	void Shader_Quad::Commit() {
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVerts) * quadVertsCount, quadVerts.get(), GL_DYNAMIC_DRAW);

		size_t j = 0;
		for (size_t i = 0; i < texsCount; i++) {
			glBindTexture(GL_TEXTURE_2D, texs[i].first);
			auto n = (GLsizei)(texs[i].second * 6);
			glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_SHORT, (GLvoid*)j);
			j += n * 2;
		}
		CheckGLError();

		sm->drawQuads += j / 2 / 6;
		sm->drawCall += texsCount;

		lastTextureId = 0;
		texsCount = 0;
		quadVertsCount = 0;
	}

	QuadVerts& Shader_Quad::DrawQuadBegin(GLTexture& tex) {
		if (quadVertsCount == maxQuadNums) {
			Commit();
		}
		if (lastTextureId != tex) {
			lastTextureId = tex;
			texs[texsCount].first = tex;
			texs[texsCount].second = 1;
			++texsCount;
		} else {
			texs[texsCount - 1].second += 1;
		}
		return quadVerts[quadVertsCount];
	}

	void Shader_Quad::DrawQuadEnd() {
		++quadVertsCount;
	}

	void Shader_Quad::DrawQuad(GLTexture& tex, QuadVerts const& qv) {
		auto&& tar = DrawQuadBegin(tex);
		memcpy(&tar, qv.data(), sizeof(qv));
		DrawQuadEnd();
	};

}
