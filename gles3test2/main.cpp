#include "esBase.h"

struct Node {
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
	void Update();
	void Draw();
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

		return 0;
	}

	XX_FORCEINLINE void Update() {
		ShowFpsInTitle();
		n->Update();
	}

	XX_FORCEINLINE void Draw() {
		glClear(GL_COLOR_BUFFER_BIT);
		n->Draw();
	}
};

inline Node::Node() {
	auto& g = Game::Instance();
	vb = g.LoadVertices(vertices.data(), sizeof(vertices));
	assert(vb);

	ib = g.LoadIndexes(indices.data(), sizeof(indices));
	assert(ib);
	ib.len = sizeof(indices);
}

void Node::Update() {
	auto& g = Game::Instance();
	x = fmodf(g.elapsedSeconds, 2.f);
	y = 0;
	z = -2;

	a = fmodf(g.elapsedSeconds * 100, 360.f);

	sz = sx = sy = 0.1 + fmodf(g.elapsedSeconds / 10, 0.9f);

	color = { 11, 11, (uint8_t)(int)(g.elapsedSeconds * 100), 0 };

	dirty = true;
}

void Node::Draw() {
	auto& g = Game::Instance();
	if (dirty) {
		esMatrixLoadIdentity(&modelMatrix);
		esTranslate(&modelMatrix, x, y, z);
		esRotate(&modelMatrix, a, 1.0, 0.0, 1.0);
		esScale(&modelMatrix, sx, sy, sz);
		esMatrixMultiply(&modelMatrix, &modelMatrix, &g.projectionMatrix);
		dirty = false;
	}

	glUseProgram(g.ps);

	glUniformMatrix4fv(g.uMvpMatrix, 1, GL_FALSE, (float*)&modelMatrix);
	glUniform4f(g.uColor, (float)color.r / 255, (float)color.g / 255, (float)color.b / 255, (float)color.a / 255);

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glVertexAttribPointer(g.iPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void*)0);
	glEnableVertexAttribArray(g.iPosition);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, 0);
}

int main() {
	Game g;
	g.vsync = 0;
	if (int r = g.Run()) {
		std::cout << "g.Run() r = " << r << ", lastErrorNumber = " << g.lastErrorNumber << ", lastErrorMessage = " << g.lastErrorMessage << std::endl;
	}
}
