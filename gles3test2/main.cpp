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

	// 用这种写法 可以不开 blend. 但是会有黑边
	//vec4 c = texture(u_sampler, v_texcoord);
	//if (c.a < 0.01) discard;
	//out_color = c;


	xx::es::Shader vs, fs;
	xx::es::Program ps;
	xx::es::VertexArrays vao;
	xx::es::Buffer vbo1, vbo2;
	xx::es::Texture tex;
	GLuint u_projection{}, u_sampler{}, in_offset_texcoord{}, in_center_scale_rotate{};

	// 个数上限
	GLuint m_max_quad_count = 5000000;

	// logic data. 通过 Init ( 个数 ) 填充
	std::vector<QuadData_center_scale_rotate> quads;

	inline int GLInit() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glViewport(0, 0, width, height);		// todo: call from window resize event
		//glClearColor(0.9f, 0.9f, 0.9f, 0.0f);

		// ps
		vs = LoadVertexShader({ vsSrc }); if (!vs) return __LINE__;
		fs = LoadFragmentShader({ fsSrc }); if (!fs) return __LINE__;
		ps = LinkProgram(vs, fs); if (!ps) return __LINE__;


		// ps args idxs
		u_projection = glGetUniformLocation(ps, "u_projection");
		u_sampler = glGetUniformLocation(ps, "u_sampler");
		in_offset_texcoord = glGetAttribLocation(ps, "in_offset_texcoord");
		in_center_scale_rotate = glGetAttribLocation(ps, "in_center_scale_rotate");


		// tex
		tex = LoadEtc2TextureFile(rootPath / "b.pkm");

		// vao
		glGenVertexArrays(1, &vao.handle);
		glBindVertexArray(vao);
		{
			// vbo instance
			glGenBuffers(1, &vbo1.handle);
			{
				glBindBuffer(GL_ARRAY_BUFFER, vbo1);
				glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData_center_scale_rotate) * m_max_quad_count, nullptr, GL_STREAM_DRAW);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// vbo shared
			glGenBuffers(1, &vbo2.handle);
			{
				glBindBuffer(GL_ARRAY_BUFFER, vbo2);
				glBufferData(GL_ARRAY_BUFFER, sizeof(offset_and_texcoord), offset_and_texcoord, GL_STATIC_DRAW);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		glBindVertexArray(0);

		check_gl_error();
		return 0;
	}

	void Update() {
		// todo: change quads logic here

		glBindBuffer(GL_ARRAY_BUFFER, vbo1);
		{
			auto buf = (QuadData_center_scale_rotate*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(QuadData_center_scale_rotate) * quads.size(),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
			memcpy(buf, quads.data(), sizeof(QuadData_center_scale_rotate) * quads.size());
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		check_gl_error();
	}

	void Draw() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(ps);
		glUniform4f(u_projection, 2.0f / width, 2.0f / height, -1.0f, -1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(u_sampler, 0);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2);
		glVertexAttribPointer(in_offset_texcoord, 4, GL_FLOAT, GL_FALSE, sizeof(QuadData_offset_texcoord), 0);
		glEnableVertexAttribArray(in_offset_texcoord);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1);
		glVertexAttribPointer(in_center_scale_rotate, 4, GL_FLOAT, GL_FALSE, sizeof(QuadData_center_scale_rotate), 0);
		glVertexAttribDivisor(in_center_scale_rotate, 1);
		glEnableVertexAttribArray(in_center_scale_rotate);

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

		quads.resize(n);
		for (auto& q : quads) {
			q.x = random_of_range(0, w);
			q.y = random_of_range(0, h);
			q.scale = random_of_range(7.5, 12.5);
			q.rotate = random_of_range(-PI, PI);
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
