// 简单模拟多贴图混用合批

#include "esBase.h"
#include "xx_string.h"

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


struct QuadData_center_color_scale_rotate_tex {
	float x;
	float y;
	float z;

	uint8_t colorR;
	uint8_t colorG;
	uint8_t colorB;
	uint8_t colorA;

	int16_t scaleX;
	int16_t scaleY;
	int16_t rotate;
	int16_t textureIndex;

	uint16_t textureRectX;
	uint16_t textureRectY;
	uint16_t textureRectWidth;
	uint16_t textureRectHeight;
};

struct QuadData_offset_texcoord {
	float x;
	float y;
};

inline static const QuadData_offset_texcoord offset_and_texcoord[4] = {
{ 0, 0 },
{ 0, 1.f },
{ 1.f, 0 },
{ 1.f, 1.f },
};

struct Game : xx::es::Context<Game> {
	static Game& Instance() { return *(Game*)instance; }

	inline static decltype(auto) vsSrc = R"--(#version 300 es
uniform vec4 u_projection;

in vec2 in_vert;

in vec3 in_center;
in vec4 in_color;
in vec4 in_scale_rotate_i;
in vec4 in_texRect;

out vec3 v_texcoord;
out vec4 v_color;

void main() {
    vec2 offset = vec2(in_vert.x - 0.5f, in_vert.y - 0.5f);
    vec2 texcoord = in_vert;

    vec2 center = in_center.xy;
	float z = in_center.z;
    vec2 scale = in_scale_rotate_i.xy;
    float rotate = in_scale_rotate_i.z;
	float i = in_scale_rotate_i.w;

    float cos_theta = cos(rotate / 3.14159265359);
    float sin_theta = sin(rotate / 3.14159265359);
    vec2 v1 = vec2(offset.x * scale.x, offset.y * scale.y);
    vec2 v2 = vec2(
       dot(v1, vec2(cos_theta, sin_theta)),
       dot(v1, vec2(-sin_theta, cos_theta))
    );
    vec2 v3 = (v2 + center) * u_projection.xy + u_projection.zw;

	v_color = in_color;
	v_texcoord = vec3(in_texRect.x + texcoord.x * in_texRect.z, in_texRect.y + texcoord.y * in_texRect.w, i);
    gl_Position = vec4(v3.x, v3.y, z, 1.0);
}
)--";
	inline static decltype(auto) fsSrc = R"--(#version 300 es
precision mediump float;

uniform sampler2D u_sampler;
in vec3 v_texcoord;
in vec4 v_color;
out vec4 out_color;
void main() {
	out_color = texture(u_sampler, v_texcoord.xy) * v_color;	// todo: v_texcoord.z texture index
}
)--";

	xx::es::Shader vs, fs;
	xx::es::Program ps;
	xx::es::VertexArrays vao;
	xx::es::Buffer vbo1, vbo2;
	std::vector<xx::es::Texture> texs;
	std::vector<std::pair<GLint, GLsizei>> texsIndexCount;
	GLuint u_projection{}, u_sampler{}, in_vert{}, in_center{}, in_scale_rotate_i{}, in_color{}, in_texRect{};

	// 个数上限
	GLuint m_max_quad_count = 5000000;

	// logic data. 通过 Init ( 个数 ) 填充
	std::vector<QuadData_center_color_scale_rotate_tex> quads;

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

		in_vert = glGetAttribLocation(ps, "in_vert");
		in_center = glGetAttribLocation(ps, "in_center");
		in_scale_rotate_i = glGetAttribLocation(ps, "in_scale_rotate_i");
		in_color = glGetAttribLocation(ps, "in_color");
		in_texRect = glGetAttribLocation(ps, "in_texRect");

		// texs
		texs.emplace_back(LoadEtc2TextureFile(rootPath / "p1.pkm"));
		texs.emplace_back(LoadEtc2TextureFile(rootPath / "p2.pkm"));

		// vao
		glGenVertexArrays(1, &vao.handle);
		glBindVertexArray(vao);
		{
			// vbo instance
			glGenBuffers(1, &vbo1.handle);
			{
				glBindBuffer(GL_ARRAY_BUFFER, vbo1);
				glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData_center_color_scale_rotate_tex) * m_max_quad_count, nullptr, GL_STREAM_DRAW);
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
		ShowFpsInTitle();

		glBindBuffer(GL_ARRAY_BUFFER, vbo1);
		{
			auto buf = (QuadData_center_color_scale_rotate_tex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(QuadData_center_color_scale_rotate_tex) * quads.size(),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
			memcpy(buf, quads.data(), sizeof(QuadData_center_color_scale_rotate_tex) * quads.size());
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		check_gl_error();
	}

	void Draw() {
		glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

		glUseProgram(ps);
		glUniform4f(u_projection, 2.0f / width, 2.0f / height, -1.0f, -1.0f);

		glUniform1i(u_sampler, 0);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2);
		glVertexAttribPointer(in_vert, 2, GL_FLOAT, GL_FALSE, sizeof(QuadData_offset_texcoord), 0);
		glEnableVertexAttribArray(in_vert);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1);

		glVertexAttribPointer(in_center, 3, GL_FLOAT, GL_FALSE, sizeof(QuadData_center_color_scale_rotate_tex), (GLvoid*)offsetof(QuadData_center_color_scale_rotate_tex, x));
		glVertexAttribDivisor(in_center, 1);
		glEnableVertexAttribArray(in_center);

		glVertexAttribPointer(in_scale_rotate_i, 4, GL_SHORT, GL_FALSE, sizeof(QuadData_center_color_scale_rotate_tex), (GLvoid*)offsetof(QuadData_center_color_scale_rotate_tex, scaleX));
		glVertexAttribDivisor(in_scale_rotate_i, 1);
		glEnableVertexAttribArray(in_scale_rotate_i);

		glVertexAttribPointer(in_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadData_center_color_scale_rotate_tex), (GLvoid*)offsetof(QuadData_center_color_scale_rotate_tex, colorR));
		glVertexAttribDivisor(in_color, 1);
		glEnableVertexAttribArray(in_color);

		glVertexAttribPointer(in_texRect, 4, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(QuadData_center_color_scale_rotate_tex), (GLvoid*)offsetof(QuadData_center_color_scale_rotate_tex, textureRectX));
		glVertexAttribDivisor(in_texRect, 1);
		glEnableVertexAttribArray(in_texRect);

		for (size_t i = 0; i < texs.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texs[i]);
			auto& ic = texsIndexCount[i];
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, ic.first, _countof(offset_and_texcoord), ic.second);
		}

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

		int texsCount = 2;	// 贴图张数
		std::array<int, 2> texsSize = { 32, 64 };	// 每张尺寸

		float z = 0;
		for (size_t i = 0; i < n; i++) {
			auto& q = quads[i];
			q.x = random_of_range(0, w);
			q.y = random_of_range(0, h);
			q.z = z;
			z += 0.0000001;		// 递增 z
			q.colorR = q.colorG = q.colorB = q.colorA = 0xFFu;
			q.textureIndex = i % texsCount;
			q.scaleX = texsSize[q.textureIndex];	// 1:1 显示
			q.scaleY = q.scaleX;
			q.rotate = 0;
			q.textureRectHeight = 0xFFFFu;
			q.textureRectWidth = 0xFFFFu;
			q.textureRectX = 0;
			q.textureRectY = 0;
		}

		// todo: 对 quads 按 textureIndex 稳定排序 并计算连续 textureIndex 的个数 起始下标
		// 先模拟下
		texsIndexCount.emplace_back(0, 100);
		texsIndexCount.emplace_back(100, 200);
	}
};

int main() {
	Game g;
	g.Init(1920, 1080, 4000000, false);
	if (int r = g.Run()) {
		std::cout << "g.Run() r = " << r << ", lastErrorNumber = " << g.lastErrorNumber << ", lastErrorMessage = " << g.lastErrorMessage << std::endl;
	}
}
