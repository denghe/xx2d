#include "pch.h"

namespace xx {

	void Shader_QuadInstance::Init(ShaderManager* sm) {
		this->sm = sm;

		v = LoadGLVertexShader({ R"(#version 300 es
uniform vec2 uCxy;	// screen center coordinate

in vec2 aVert;	// { 0, 0 }, { 0, 1.f }, { 1.f, 0 }, { 1.f, 1.f }

in vec4 aPosScaleRadians;
in vec4 aColor;
in vec4 aTexRect;

out vec2 vTexCoord;
out vec4 vColor;

void main() {
    vec2 pos = aPosScaleRadians.xy;
    float scale = aPosScaleRadians.z;
	float radians = aPosScaleRadians.w;

    vec2 anchor = vec2((aVert.x - 0.5f) * scale, (aVert.y - 0.5f) * scale);

    float c = cos(radians / 3.14159265359);
    float s = sin(radians / 3.14159265359);

    vec2 v = pos + vec2(
       dot(anchor, vec2(c, s)),
       dot(anchor, vec2(-s, c))
    );
    vec2 vv = (v + pos) / uCxy;
    gl_Position = vec4(vv.x, vv.y, 0, 1);

	vColor = aColor;
	vTexCoord = vec2(aTexRect.x + aVert.x * aTexRect.z, aTexRect.y + aVert.y * aTexRect.w);
})"sv });

		f = LoadGLFragmentShader({ R"(#version 300 es
precision highp float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord);
})"sv });

		p = LinkGLProgram(v, f);

		uCxy = glGetUniformLocation(p, "uCxy");
		uTex0 = glGetUniformLocation(p, "uTex0");

		aVert = glGetAttribLocation(p, "aVert");
		aPosScaleRadians = glGetAttribLocation(p, "aPosScaleRadians");
		aColor = glGetAttribLocation(p, "aColor");
		aTexRect = glGetAttribLocation(p, "aTexRect");
		CheckGLError();

		glGenVertexArrays(1, &va.Ref());
		glBindVertexArray(va);

		glGenBuffers(1, (GLuint*)&ib);

		glBindBuffer(GL_ARRAY_BUFFER, ib);
		static const float verts[8] = { 0, 0,    0, 1.f,    1.f, 0,    1.f, 1.f };
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glVertexAttribPointer(aVert, 2, GL_FLOAT, GL_FALSE, 8, 0);
		glEnableVertexAttribArray(aVert);

		glGenBuffers(1, (GLuint*)&vb);

		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glVertexAttribPointer(aPosScaleRadians, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstanceData), 0);
		glEnableVertexAttribArray(aPosScaleRadians);

		glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, color));
		glEnableVertexAttribArray(aColor);

		glVertexAttribPointer(aTexRect, 4, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(QuadInstanceData), (GLvoid*)offsetof(QuadInstanceData, texRectX));
		glEnableVertexAttribArray(aTexRect);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CheckGLError();
	}

	void Shader_QuadInstance::Begin() {
		if (sm->cursor != index) {
			// here can check shader type for combine batch
			sm->shaders[sm->cursor]->End();
			sm->cursor = index;
		}

		glUseProgram(p);
		glUniform1i(uTex0, 0);
		glUniform2f(uCxy, engine.w / 2, engine.h / 2);

		glBindVertexArray(va);
	}

	void Shader_QuadInstance::End() {
		if (quadVertsCount) {
			Commit();
		}
	}

	void Shader_QuadInstance::Commit() {
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadInstanceData) * quadVertsCount, quadVerts.get(), GL_DYNAMIC_DRAW);

		size_t j = 0;
		for (size_t i = 0; i < texsCount; i++) {
			glBindTexture(GL_TEXTURE_2D, texs[i].first);
			auto n = (GLsizei)texs[i].second;
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, j, 4, n);
			j += n * sizeof(QuadInstanceData);
		}
		CheckGLError();

		sm->drawQuads += j;
		sm->drawCall += texsCount;

		lastTextureId = 0;
		texsCount = 0;
		quadVertsCount = 0;
	}

	QuadInstanceData& Shader_QuadInstance::DrawQuadBegin(GLTexture& tex) {
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

	void Shader_QuadInstance::DrawQuadEnd() {
		++quadVertsCount;
	}

	void Shader_QuadInstance::DrawQuad(GLTexture& tex, QuadInstanceData const& qv) {
		auto&& tar = DrawQuadBegin(tex);
		memcpy(&tar, &qv, sizeof(qv));
		DrawQuadEnd();
	};

}
