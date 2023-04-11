#include "xx2d.h"

namespace xx {

	void Shader_Spine::Init(ShaderManager* sm) {
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
	gl_Position = vec4(aPos * uCxy, 0, 1);
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

		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYUVRGBA8), 0);
		glEnableVertexAttribArray(aPos);
		glVertexAttribPointer(aTexCoord, 2, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, u));
		glEnableVertexAttribArray(aTexCoord);
		glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, r));
		glEnableVertexAttribArray(aColor);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CheckGLError();
	}

	void Shader_Spine::Begin() {
		if (sm->cursor != index) {
			// here can check shader type for combine batch
			sm->shaders[sm->cursor]->End();
			sm->cursor = index;
		}

		engine.GLEnableBlend();

		glUseProgram(p);
		glActiveTexture(GL_TEXTURE0/* + textureUnit*/);
		glUniform1i(uTex0, 0);
		glUniform2f(uCxy, 2 / engine.w, 2 / engine.h);

		glBindVertexArray(va);
	}

	void Shader_Spine::End() {
		if (texsCount) {
			Commit();
		}
	}

	void Shader_Spine::Commit() {
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(XYUVRGBA8) * vertsCount, verts.get(), GL_STREAM_DRAW);

		GLsizei j = 0;
		for (size_t i = 0; i < texsCount; i++) {
			glBindTexture(GL_TEXTURE_2D, texs[i].first);
			glDrawArrays(GL_TRIANGLES, j, texs[i].second);
			j += texs[i].second;
		}
		CheckGLError();

		sm->drawVerts += j;
		sm->drawCall += texsCount;

		lastTextureId = 0;
		texsCount = 0;
		vertsCount = 0;
	}

	XYUVRGBA8* Shader_Spine::Draw(GLTexture& tex, size_t const& numVerts) {
		assert(numVerts <= maxVertNums);
		if (vertsCount + numVerts > maxVertNums) {
			Commit();
		}
		if (lastTextureId != tex) {
			lastTextureId = tex;
			texs[texsCount].first = tex;
			texs[texsCount].second = numVerts;
			++texsCount;
		} else {
			texs[texsCount - 1].second += numVerts;
		}
		auto r = &verts[vertsCount];
		vertsCount += numVerts;
		return r;
	}
}
