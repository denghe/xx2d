#include "esBase.h"

#ifndef NDEBUG
void check_gl_error_at(const char* file, int line, const char* func) {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("OpenGL error: 0x%04x, at %s:%d, %s\n", err, file, line, func);
		abort();
	}
}

#define check_gl_error() check_gl_error_at(__FILE__, __LINE__, __FUNCTION__)
#else
#define check_gl_error() ((void)0)
#endif

struct QuadData_center_scale_rotate
{
	GLfloat x;
	GLfloat y;
	GLfloat scale;
	GLfloat rotate;
};

struct QuadData_offset_texcoord {
	GLfloat x;
	GLfloat y;
	GLfloat u;
	GLfloat v;
};

inline static const QuadData_offset_texcoord offset_and_texcoord[6] = {
{ -0.5f, -0.5f, 0.0f, 0.0f },
{ 0.5f, -0.5f, 1.0f, 0.0f },
{ 0.5f, 0.5f, 1.0f, 1.0f },

{ 0.5f, 0.5f, 1.0f, 1.0f },
{ -0.5f, 0.5f, 0.0f, 1.0f },
{ -0.5f, -0.5f, 0.0f, 0.0f },
};

struct Game : xx::es::Context<Game> {
	static Game& Instance() { return *(Game*)instance; }

	inline static decltype(auto) vsSrc = R"--(#version 300 es
uniform vec4 u_projection;
in vec4 in_offset_texcoord;
in vec4 in_center_scale_rotate;
out vec2 v_texcoord;
void main() {
    vec2 offset = in_offset_texcoord.xy;
    vec2 texcoord = in_offset_texcoord.zw;
    vec2 center = in_center_scale_rotate.xy;
    float scale = in_center_scale_rotate.z;
    float rotate = in_center_scale_rotate.w;

    float cos_theta = cos(rotate);
    float sin_theta = sin(rotate);
    vec2 v1 = offset * scale;
    vec2 v2 = vec2(
       dot(v1, vec2(cos_theta, sin_theta)),
       dot(v1, vec2(-sin_theta, cos_theta))
       );
    vec2 v3 = (v2 + center) * u_projection.xy + u_projection.zw;

    v_texcoord = texcoord;
    gl_Position = vec4(v3.x, v3.y, 0.0, 1.0);
}
)--";
	inline static decltype(auto) fsSrc = R"--(#version 300 es
precision mediump float;

uniform sampler2D u_sampler;
in vec2 v_texcoord;
out vec4 out_color;
void main() {
    out_color = texture(u_sampler, v_texcoord);
}
)--";

	xx::es::Shader vs, fs;
	xx::es::Program ps;
	xx::es::VertexArrays m_vao;
	xx::es::Buffer m_vbo_per_instance, m_vbo_shared;
	xx::es::Texture tex;
	GLuint u_projection{}, u_sampler{}, in_offset_texcoord{}, in_center_scale_rotate{};

	// 个数上限
	GLuint m_max_quad_count = 5000000;

	// logic data. 通过 Init ( 个数 ) 填充
	std::vector<QuadData_center_scale_rotate> quads;

	inline int GLInit() {
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0, 0, width, height);		// todo: call from window resize event
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

		// ps
		vs = LoadVertexShader({ vsSrc }); if (!vs) return __LINE__;
		fs = LoadFragmentShader({ fsSrc }); if (!fs) return __LINE__;
		ps = LinkProgram(vs, fs); if (!ps) return __LINE__;

		glUseProgram(ps);

		// ps args idxs
		u_projection = glGetAttribLocation(ps, "u_projection");
		u_sampler = glGetAttribLocation(ps, "u_sampler");
		in_offset_texcoord = glGetAttribLocation(ps, "in_offset_texcoord");
		in_center_scale_rotate = glGetAttribLocation(ps, "in_center_scale_rotate");
		check_gl_error();

		// uniform args: matrix
		glUniform4f(u_projection,
			2.0f / width,
			2.0f / height,
			-1.0f,
			-1.0f);

		// uniform args: tex
		tex = LoadEtc2TextureFile(rootPath / "b.pkm");
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(u_sampler, 0);
		check_gl_error();

		// vao
		glGenVertexArrays(1, &m_vao.handle);
		glBindVertexArray(m_vao);
		check_gl_error();

		// vbo instance
		glGenBuffers(1, &m_vbo_per_instance.handle);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_per_instance);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData_center_scale_rotate) * m_max_quad_count, nullptr, GL_STREAM_DRAW);
		check_gl_error();

		// vbo shared
		glGenBuffers(1, &m_vbo_shared.handle);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_shared);
		glBufferData(GL_ARRAY_BUFFER, sizeof(offset_and_texcoord), offset_and_texcoord, GL_STATIC_DRAW);
		check_gl_error();

		return 0;
	}

	void Update() {
		ShowFpsInTitle();

		// todo: logic here

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_per_instance);
		auto buf = (QuadData_center_scale_rotate*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(QuadData_center_scale_rotate) * quads.size(),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		memcpy(buf, quads.data(), sizeof(QuadData_center_scale_rotate) * quads.size());
		glUnmapBuffer(GL_ARRAY_BUFFER);
		check_gl_error();
	}

	void Draw() {
		glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(m_vao);
		check_gl_error();

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_shared);
		glVertexAttribPointer(in_offset_texcoord, 4, GL_FLOAT, GL_FALSE, sizeof(QuadData_offset_texcoord), 0);
		glEnableVertexAttribArray(in_offset_texcoord);
		check_gl_error();

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_per_instance);
		glVertexAttribPointer(in_center_scale_rotate, 4, GL_FLOAT, GL_FALSE, sizeof(QuadData_center_scale_rotate), 0);
		glVertexAttribDivisor(in_center_scale_rotate, 1);
		glEnableVertexAttribArray(in_center_scale_rotate);
		check_gl_error();

		glBindTexture(GL_TEXTURE_2D, tex);
		check_gl_error();

		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)quads.size());
		check_gl_error();
	}

	void Init(int const& w, int const& h, GLuint const& n, bool vsync) {
		assert(n > 0 && n <= m_max_quad_count);
		this->width = w;
		this->height = h;
		this->vsync = vsync ? 1 : 0;

		auto random_of_range = [](GLfloat min, GLfloat max) {
			return min + (max - min) * (GLfloat(rand()) / RAND_MAX);
		};

		GLfloat min_x = 0.0f;
		GLfloat max_x = GLfloat(w);
		GLfloat min_y = 0.0f;
		GLfloat max_y = GLfloat(h);
		GLfloat min_scale = 15.0f;
		GLfloat max_scale = 25.0f;
		GLfloat min_rotate = -PI * 5;
		GLfloat max_rotate = PI * 5;

		quads.resize(n);
		for (auto& q : quads) {
			q.x = random_of_range(min_x, max_x);
			q.y = random_of_range(min_y, max_y);
			q.scale = random_of_range(min_scale, max_scale);
			q.rotate = random_of_range(min_rotate, max_rotate);
		}
	}
};


int main() {
	Game g;
	g.Init(1920, 1080, 1000000, false);
	if (int r = g.Run()) {
		std::cout << "g.Run() r = " << r << ", lastErrorNumber = " << g.lastErrorNumber << ", lastErrorMessage = " << g.lastErrorMessage << std::endl;
	}
}













//#include "esBase.h"
//
//struct Vec2f {
//	GLfloat x, y;
//};
//struct Vec3f : Vec2f {
//	GLfloat z;
//};
//struct Color4b {
//	GLubyte r, g, b, a;
//};
//struct XyzColorUv {
//	Vec3f pos;
//	Color4b color;
//	Vec2f uv;
//};
//
//struct Game : xx::es::Context<Game> {
//	static Game& Instance() { return *(Game*)instance; }
//	static const int n = 100000;
//
//	inline static decltype(auto) vsSrc = R"--(
//#version 300 es
//
////uniform mat4 uMatrix;
//
//in vec3 iPos;
//in vec4 iColor;
//in vec2 iUV;
//
//out vec4 vColor;
//out vec2 vUV;
//
//void main() {
//	//gl_Position = uMatrix * vec4(iPos, 1.0f);
//	gl_Position = vec4(iPos, 1.0f);
//	vColor = iColor;
//	vUV = iUV;
//}
//)--";
//	inline static decltype(auto) fsSrc = R"--(
//#version 300 es
//precision mediump float;
//
//uniform sampler2D uTexture;
//
//in vec4 vColor;
//in vec2 vUV;
//
//out vec4 oColor;
//
//void main() {
//   oColor = vColor * texture2D(uTexture, vUV);
//}
//)--";
//
//	xx::es::Shader vs, fs;
//	xx::es::Program ps;
//	GLuint iPos{}, iColor{}, iUV{}, uMatrix{}, uTexture{};
//	xx::es::Buffer vb, ib;
//	std::vector<XyzColorUv> verts;
//	std::vector<GLushort> idxs;
//	xx::es::Texture t;
//
//	inline int GLInit() {
//		vs = LoadVertexShader({ vsSrc });
//		if (!vs) return __LINE__;
//
//		fs = LoadFragmentShader({ fsSrc });
//		if (!fs) return __LINE__;
//
//		ps = LinkProgram(vs, fs);
//		if (!ps) return __LINE__;
//
//		iPos = glGetAttribLocation(ps, "iPos");
//		iColor = glGetAttribLocation(ps, "iColor");
//		iUV = glGetAttribLocation(ps, "iUV");
//		uMatrix = glGetUniformLocation(ps, "uMatrix");
//		uTexture = glGetUniformLocation(ps, "uTexture");
//
//		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
//
//		// todo: call from window resize event
//		glViewport(0, 0, width, height);
//
//		//vb = LoadVertices(verts.data(), verts.size() * sizeof(XyzColorUv));
//		glGenBuffers(1, &vb.handle);
//		assert(vb);
//		//ib = LoadIndexes(idxs.data(), idxs.size() * sizeof(GLushort));
//		glGenBuffers(1, &ib.handle);
//		assert(ib);
//		{
//			for (size_t i = 0; i < n; i++) {
//				GLushort b = i * 4;
//				idxs.insert(idxs.begin(), { (GLushort)(b + 0), (GLushort)(b + 1), (GLushort)(b + 2), (GLushort)(b + 2), (GLushort)(b + 3), (GLushort)(b + 0) });
//			}
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib); check_gl_error();
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)(idxs.size() * sizeof(GLushort)), idxs.data(), GL_STATIC_DRAW); check_gl_error();
//		}
//
//		t = LoadEtc2TextureFile(rootPath / "b.pkm");
//		assert(t);
//		return 0;
//	}
//
//	XX_FORCEINLINE void Update() {
//		ShowFpsInTitle();
//
//		if (verts.empty()) {
//			for (size_t i = 0; i < n; i++) {
//				Color4b c{ 255,111,111,255 };
//				{
//					auto&& o = verts.emplace_back();
//					o.pos = { 0,0,0 };
//					o.color = c;
//					o.uv = { 0,0 };
//				}
//				{
//					auto&& o = verts.emplace_back();
//					o.pos = { 1,0,0 };
//					o.color = c;
//					o.uv = { 1,0 };
//				}
//				{
//					auto&& o = verts.emplace_back();
//					o.pos = { 1,1,0 };
//					o.color = c;
//					o.uv = { 1,1 };
//				}
//				{
//					auto&& o = verts.emplace_back();
//					o.pos = { 0,1,0 };
//					o.color = c;
//					o.uv = { 0,1 };
//				}
//			}
//
//			glBindBuffer(GL_ARRAY_BUFFER, vb); check_gl_error();
//			glBufferData(GL_ARRAY_BUFFER, (GLsizei)(verts.size() * sizeof(XyzColorUv)), verts.data(), GL_STATIC_DRAW); check_gl_error();
//		}
//	}
//
//	XX_FORCEINLINE void Draw() {
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		glUseProgram(ps); check_gl_error();
//		glBindBuffer(GL_ARRAY_BUFFER, vb); check_gl_error();
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib); check_gl_error();
//
//		glEnableVertexAttribArray(iPos); check_gl_error();
//		glVertexAttribPointer(iPos, 3, GL_FLOAT, GL_FALSE, sizeof(XyzColorUv), (GLvoid*)offsetof(XyzColorUv, pos)); check_gl_error();
//		glEnableVertexAttribArray(iColor); check_gl_error();
//		glVertexAttribPointer(iColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XyzColorUv), (GLvoid*)offsetof(XyzColorUv, color)); check_gl_error();
//		glEnableVertexAttribArray(iUV); check_gl_error();
//		glVertexAttribPointer(iUV, 2, GL_FLOAT, GL_FALSE, sizeof(XyzColorUv), (GLvoid*)offsetof(XyzColorUv, uv)); check_gl_error();
//
//		glActiveTexture(GL_TEXTURE0); check_gl_error();
//		glBindTexture(GL_TEXTURE_2D, t); check_gl_error();
//		glUniform1i(uTexture, 0); check_gl_error();
//
//		glDrawElements(GL_TRIANGLES, (GLsizei)idxs.size(), GL_UNSIGNED_SHORT, 0); check_gl_error();
//	}
//};
//
//
//
//
//int main() {
//	Game g;
//	g.vsync = 0;
//	g.width = 256;
//	g.height = 256;
//	if (int r = g.Run()) {
//		std::cout << "g.Run() r = " << r << ", lastErrorNumber = " << g.lastErrorNumber << ", lastErrorMessage = " << g.lastErrorMessage << std::endl;
//	}
//}
