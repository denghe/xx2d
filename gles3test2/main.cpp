#include "esBase.h"
#include "texLoader.h"

struct Node {
	std::vector<xx::Shared<Node>> children;

	std::array<GLfloat, 24 * 3> vertices = {
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
	std::array<GLushort, 12 * 3> indices = {
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
	xx::es::Buffer vb, ib;
	xx::es::Texture t;

	bool dirty = false;
	float x = 0;
	float y = 0;
	float z = 0;
	float sx = 1.0f;
	float sy = 1.0f;
	float sz = 1.0f;
	float a = 0;
	xx::es::Color4b color = { 255, 127, 127, 0 };

	xx::es::Matrix modelMatrix;

	Node();
	std::function<void()> Update;
	void Draw(xx::es::Matrix const& parentMatrix);
};

struct Game : xx::es::Context<Game> {
	static Game& Instance() { return *(Game*)instance; }

	GLint imat = 0;

	xx::es::Shader vs, fs;
	xx::es::Program ps;
	inline static int iPosition = 0;
	inline static int uMvpMatrix = 0;
	inline static int uColor = 0;
	xx::es::Matrix projectionMatrix;

	xx::Shared<Node> n;

	// 使用示例： if (int r = CheckGLError(__LINE__)) return r;

	inline int GLInit() {
		vs = LoadVertexShader({ R"--(
#version 300 es
in vec4 iPosition;
uniform vec4 uColor;
uniform mat4 uMvpMatrix;
out vec4 vColor;
void main() {
	gl_Position = uMvpMatrix * iPosition;
	vColor = uColor;
}
)--" });
		if (!vs) return __LINE__;

		fs = LoadFragmentShader({ R"--(
#version 300 es
precision mediump float;
in vec4 vColor;
out vec4 oColor;
void main() {
   oColor = vColor;
}
)--" });
		if (!fs) return __LINE__;

		ps = LinkProgram(vs, fs);
		if (!ps) return __LINE__;

		iPosition = glGetAttribLocation(ps, "iPosition");
		uMvpMatrix = glGetUniformLocation(ps, "uMvpMatrix");
		uColor = glGetUniformLocation(ps, "uColor");

		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

		glDisable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		if (int r = CheckGLError(__LINE__)) return r;

		// todo: call from window resize event
		glViewport(0, 0, width, height);

		auto aspect = (GLfloat)width / (GLfloat)height;
		esMatrixLoadIdentity(&projectionMatrix);
		esOrtho(&projectionMatrix, -aspect, aspect, 1, -1, -10, 10);



		n.Emplace();
		n->Update = [n = n.pointer, this]{
			n->x = fmodf(elapsedSeconds, 2.f);
			n->y = 0;
			n->z = -2;

			n->a = fmodf(elapsedSeconds * 100, 360.f);

			n->color = { 11, 11, (uint8_t)(int)(elapsedSeconds * 100), 0 };

			n->dirty = true;

			for (auto& c : n->children) {
				c->Update();
			}
		};
		for (size_t i = 0; i < 10; i++) {
			auto&& n2 = n->children.emplace_back().Emplace();
			n2->Update = [i = i, n = n2.pointer, this]{
				n->y = 0.1f + (float)i * 0.1f;
				n->a = fmodf(elapsedSeconds * 100, 360.f);
				n->sz = n->sx = n->sy = 0.1f + fmodf(elapsedSeconds / 10, 0.5f);
				n->dirty = true;
			};
		}

		return 0;
	}

	XX_FORCEINLINE void Update() {
		ShowFpsInTitle();
		n->Update();
	}

	XX_FORCEINLINE void Draw() {
		glClear(GL_COLOR_BUFFER_BIT);
		n->Draw(projectionMatrix);
		assert(!CheckGLError(__LINE__));
	}
};

inline Node::Node() {
	auto& g = Game::Instance();
	vb = g.LoadVertices(vertices.data(), sizeof(vertices));
	assert(vb);

	ib = g.LoadIndexes(indices.data(), sizeof(indices));
	assert(ib);
	ib.ud = sizeof(indices);

	t = g.LoadEtc2TextureFile(g.rootPath / "all.pkm");
	assert(t);
}

// 指令合并思路：如果相同 ps 且 vb ib 一致, 似乎可跳过这部分代码. 
// 如果 ps 不一致 则清0 vb ib. 如果中途会 释放资源，则可能需要每帧清 0
inline GLuint _ps = 0, _vb = 0, _ib = 0;

void Node::Draw(xx::es::Matrix const& parentMatrix) {
	auto& g = Game::Instance();

	if (dirty) {
		dirty = false;
		xx::es::Matrix m;
		esMatrixLoadIdentity(&m);
		if (x != 0.f || y != 0.f || z != 0.f) {
			esTranslate(&m, x, y, z);
		}
		if (a != 0.f) {
			esRotate(&m, a, 1.0, 0.0, 1.0);
		}
		if (sx != 1.f || sy != 1.f || sz != 1.f) {
			esScale(&m, sx, sy, sz);
		}
		esMatrixMultiply(&modelMatrix, &m, &parentMatrix);
	}

	if (_ps != g.ps) {
		_ps = g.ps;
		_vb = _ib = 0;
		glUseProgram(g.ps);
	}
	if (_vb != vb) {
		_vb = vb;
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glVertexAttribPointer(g.iPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void*)0);
		glEnableVertexAttribArray(g.iPosition);
	}
	if (_ib != ib) {
		_ib = ib;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	}

	glUniformMatrix4fv(g.uMvpMatrix, 1, GL_FALSE, (float*)&modelMatrix);
	glUniform4f(g.uColor, (float)color.r / 255, (float)color.g / 255, (float)color.b / 255, (float)color.a / 255);

	glDrawElements(GL_TRIANGLES, ib.ud, GL_UNSIGNED_SHORT, 0);

	for (auto& c : children) {
		c->Draw(modelMatrix);
	}
}





int main() {
	Game g;
	g.vsync = 0;
	if (int r = g.Run()) {
		std::cout << "g.Run() r = " << r << ", lastErrorNumber = " << g.lastErrorNumber << ", lastErrorMessage = " << g.lastErrorMessage << std::endl;
	}
}
