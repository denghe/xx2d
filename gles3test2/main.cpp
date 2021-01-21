#include "esBase.h"

struct Vec2f {
	GLfloat x, y;
};
struct Vec3f : Vec2f {
	GLfloat z;
};
struct Color4b {
	GLubyte r, g, b, a;
};
struct XyzColorUv {
	Vec3f pos;
	Color4b color;
	Vec2f uv;
};

struct Game : xx::es::Context<Game> {
	static Game& Instance() { return *(Game*)instance; }

	inline static decltype(auto) vsSrc = R"--(
#version 300 es

//uniform mat4 uMatrix;

in vec3 iPos;
in vec4 iColor;
in vec2 iUV;

out vec4 vColor;
out vec2 vUV;

void main() {
	//gl_Position = uMatrix * vec4(iPos, 1.0f);
	gl_Position = vec4(iPos, 1.0f);
	vColor = iColor;
	vUV = iUV;
}
)--";
	inline static decltype(auto) fsSrc = R"--(
#version 300 es
precision mediump float;

uniform sampler2D uTexture;

in vec4 vColor;
in vec2 vUV;

out vec4 oColor;

void main() {
   oColor = vColor * texture2D(uTexture, vUV);
}
)--";

	xx::es::Shader vs, fs;
	xx::es::Program ps;
	GLuint iPos{}, iColor{}, iUV{}, uMatrix{}, uTexture{};
	xx::es::Buffer vb, ib;
	std::vector<XyzColorUv> verts;
	std::vector<GLushort> idxs;
	xx::es::Texture t;

	inline int GLInit() {
		vs = LoadVertexShader({ vsSrc });
		if (!vs) return __LINE__;

		fs = LoadFragmentShader({ fsSrc });
		if (!fs) return __LINE__;

		ps = LinkProgram(vs, fs);
		if (!ps) return __LINE__;

		iPos = glGetAttribLocation(ps, "iPos");
		iColor = glGetAttribLocation(ps, "iColor");
		iUV = glGetAttribLocation(ps, "iUV");
		uMatrix = glGetUniformLocation(ps, "uMatrix");
		uTexture = glGetUniformLocation(ps, "uTexture");

		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

		// todo: call from window resize event
		glViewport(0, 0, width, height);

		//vb = LoadVertices(verts.data(), verts.size() * sizeof(XyzColorUv));
		glGenBuffers(1, &vb.handle);
		assert(vb);
		//ib = LoadIndexes(idxs.data(), idxs.size() * sizeof(GLushort));
		glGenBuffers(1, &ib.handle);
		assert(ib);
		{
			for (size_t i = 0; i < 10000; i++) {
				GLushort b = i * 4;
				idxs.insert(idxs.begin(), { (GLushort)(b + 0), (GLushort)(b + 1), (GLushort)(b + 2), (GLushort)(b + 2), (GLushort)(b + 3), (GLushort)(b + 0) });
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib); assert(!CheckGLError(__LINE__));
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)(idxs.size() * sizeof(GLushort)), idxs.data(), GL_STATIC_DRAW); assert(!CheckGLError(__LINE__));
		}

		t = LoadEtc2TextureFile(rootPath / "all.pkm");
		assert(t);
		return 0;
	}

	XX_FORCEINLINE void Update() {
		ShowFpsInTitle();

		if (verts.empty()) {
			for (size_t i = 0; i < 10000; i++) {
				Color4b c{ 255,111,111,255 };
				{
					auto&& o = verts.emplace_back();
					o.pos = { 0,0,0 };
					o.color = c;
					o.uv = { 0,0 };
				}
				{
					auto&& o = verts.emplace_back();
					o.pos = { 1,0,0 };
					o.color = c;
					o.uv = { 1,0 };
				}
				{
					auto&& o = verts.emplace_back();
					o.pos = { 1,1,0 };
					o.color = c;
					o.uv = { 1,1 };
				}
				{
					auto&& o = verts.emplace_back();
					o.pos = { 0,1,0 };
					o.color = c;
					o.uv = { 0,1 };
				}
			}

			glBindBuffer(GL_ARRAY_BUFFER, vb); assert(!CheckGLError(__LINE__));
			glBufferData(GL_ARRAY_BUFFER, (GLsizei)(verts.size() * sizeof(XyzColorUv)), verts.data(), GL_STATIC_DRAW); assert(!CheckGLError(__LINE__));
		}
	}

	XX_FORCEINLINE void Draw() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(ps); assert(!CheckGLError(__LINE__));
		glBindBuffer(GL_ARRAY_BUFFER, vb); assert(!CheckGLError(__LINE__));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib); assert(!CheckGLError(__LINE__));

		glEnableVertexAttribArray(iPos); assert(!CheckGLError(__LINE__));
		glVertexAttribPointer(iPos, 3, GL_FLOAT, GL_FALSE, sizeof(XyzColorUv), (GLvoid*)offsetof(XyzColorUv, pos)); assert(!CheckGLError(__LINE__));
		glEnableVertexAttribArray(iColor); assert(!CheckGLError(__LINE__));
		glVertexAttribPointer(iColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XyzColorUv), (GLvoid*)offsetof(XyzColorUv, color)); assert(!CheckGLError(__LINE__));
		glEnableVertexAttribArray(iUV); assert(!CheckGLError(__LINE__));
		glVertexAttribPointer(iUV, 2, GL_FLOAT, GL_FALSE, sizeof(XyzColorUv), (GLvoid*)offsetof(XyzColorUv, uv)); assert(!CheckGLError(__LINE__));

		glActiveTexture(GL_TEXTURE0); assert(!CheckGLError(__LINE__));
		glBindTexture(GL_TEXTURE_2D, t); assert(!CheckGLError(__LINE__));
		glUniform1i(uTexture, 0); assert(!CheckGLError(__LINE__));

		glDrawElements(GL_TRIANGLES, (GLsizei)idxs.size(), GL_UNSIGNED_SHORT, 0); assert(!CheckGLError(__LINE__));
	}
};




int main() {
	Game g;
	g.vsync = 0;
	if (int r = g.Run()) {
		std::cout << "g.Run() r = " << r << ", lastErrorNumber = " << g.lastErrorNumber << ", lastErrorMessage = " << g.lastErrorMessage << std::endl;
	}
}
