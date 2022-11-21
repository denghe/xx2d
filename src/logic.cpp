#include "pch.h"
#include "logic.h"

inline static auto vsSrc = R"--(
#version 300 es
layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aPosition;
out vec4 vColor;
void main()
{
   vColor = aColor;
   gl_Position = aPosition;
}
)--"sv;

inline static auto fsSrc = R"--(
#version 300 es
precision mediump float;
in vec4 vColor;
out vec4 oFragColor;
void main()
{
   oFragColor = vColor;
}
)--"sv;

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

void Logic::GLInit() {
	CheckGLError();
	v = LoadVertexShader({ vsSrc });
	f = LoadFragmentShader({ fsSrc });
	p = LinkProgram(v, f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);								CheckGLError();
}

void Logic::Update(float delta) {
	assert(w >= 0 && h >= 0);	// 防止 glViewport 出错
	glViewport(0, 0, w, h);												CheckGLError();
	glClear(GL_COLOR_BUFFER_BIT);										CheckGLError();
	assert(p);
	glUseProgram(p);													CheckGLError();
	glVertexAttrib4fv(0, color.data());									CheckGLError();
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, verts.data());	CheckGLError();
	glEnableVertexAttribArray(1);										CheckGLError();
	glDrawArrays(GL_TRIANGLES, 0, 3);									CheckGLError();
}

/*
* glEnable
* glDisable
* gIisEnabled
* 
GL_BLEND
GL_CULL_FACE
GL_DEPTH_TEST
GL_DITHER
GL_POLYGON_OFFSET_FILL
GL_PRIMITIVE_RESTART_FIXED_INDEX
GL_RASTERIZER_DISCARD
GL_SAMPLE_ALPHA_TO_COVERAGE
GL_SAMPLE_COVERAGE
GL_SCISSOR_TEST
GL_STENCIL_TEST
*/


/*

Logic::Logic() {
	v = LoadVertexShader({ R"(#version 300 es
precision highp float;
uniform mat4 uMVPMatrix;

in vec4 aPos;
in vec4 aColor;
in vec2 aTexCoord;

out mediump vec4 vColor;
out mediump vec2 vTexCoord;

void main() {
    gl_Position = uMVPMatrix * aPos;
	vColor = aColor;
	vTexCoord = aTexCoord;
})" });
	f = LoadFragmentShader({ R"(#version 300 es
precision mediump float;
uniform sampler2D uTex0;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
	oColor = vColor * texture(uTex0, vTexCoord);
}
)" });
	p = LinkProgram(v, f);

	glBindAttribLocation(p, 0, "aPos");
	glBindAttribLocation(p, 1, "aColor");
	glBindAttribLocation(p, 2, "aTexCoord");

	uTex0 = glGetUniformLocation(p, "uTex0");
	uMVPMatrix = glGetUniformLocation(p, "uMVPMatrix");
}


	//glUseProgram(p);

	//glUniform1i(uTex0, 0);
	//glUniform???(uMVPMatrix, 0);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, t);
*/