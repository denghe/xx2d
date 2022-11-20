//#include "esBase.h"
//#ifdef NDEBUG
//#include <omp.h>
//#endif
//#include <thread>
//#include <xx_ptr.h>
//
//#ifndef NDEBUG
//void check_gl_error_at(const char* file, int line, const char* func) {
//	GLenum err = glGetError();
//	if (err != GL_NO_ERROR) {
//		printf("OpenGL error: 0x%04x, at %s:%d, %s\n", err, file, line, func);
//		abort();
//	}
//}
//
//#define check_gl_error() check_gl_error_at(__FILE__, __LINE__, __FUNCTION__)
//#else
//#define check_gl_error() ((void)0)
//#endif
//
//static_assert(sizeof(GLfloat) == sizeof(float));
//
//// 2d变换矩阵
//using Mat = std::array<float, 3*3>;
//
//// 每个矩形的 实例上下文
//// todo: anchor point support?
//struct QuadInstance {
//	// mvp( 3 * 3 )
//	Mat mat;
//
//	// 坐标
//	float posX;
//	float posY;
//	float posZ;
//	// 贴图数组下标
//	float textureIndex;
//
//	// 图块范围 0 ~ 1 值域
//	float textureRectX;
//	float textureRectY;
//	float textureRectWidth;
//	float textureRectHeight;
//
//	// 混淆色
//	uint8_t colorR;
//	uint8_t colorG;
//	uint8_t colorB;
//	uint8_t colorA;
//	// 缩放
//	float scaleX;
//	float scaleY;
//	// 围绕z轴的旋转角度
//	float angleZ;
//};
//
//// 所有矩形共享的 顶点结构
//struct QuadIndex {
//	float x;
//	float y;
//	float u;
//	float v;
//};
//
//// 6个顶点数据
//inline static const QuadIndex quadIndexs[6] = {
//{ -0.5f, -0.5f, 0.0f, 0.0f },
//{ 0.5f, -0.5f, 1.0f, 0.0f },
//{ 0.5f, 0.5f, 1.0f, 1.0f },
//
//{ 0.5f, 0.5f, 1.0f, 1.0f },
//{ -0.5f, 0.5f, 0.0f, 1.0f },
//{ -0.5f, -0.5f, 0.0f, 0.0f },
//};
//
//// 和上面 quad 结构配合的 shader代码
//inline static decltype(auto) vsSrc = R"--(#version 300 es
//in vec4 in_index;
//
//in mat3 in_mat;
//in vec4 in_posXYZtextureIndex;
//in vec4 in_texRect;
//in vec4 in_colorRGBA;
//in vec3 in_scaleXYangleZ;
//
//out vec3 v_texcoord;
//out vec4 v_color;
//
//void main() {
//    vec2 offset = in_index.xy;
//    vec2 texcoord = in_index.zw;
//
//    vec2 center = in_posXYZtextureIndex.xy;
//	float z = in_posXYZtextureIndex.z;
//	float texidx = in_posXYZtextureIndex.w;
//
//    vec2 scale = in_scaleXYangleZ.xy;
//    float rotate = in_scaleXYangleZ.z;
//
//    float cos_theta = cos(rotate);
//    float sin_theta = sin(rotate);
//
//    vec2 v1 = vec2(offset.x * scale.x, offset.y * scale.y);
//    vec2 v2 = vec2(
//       dot(v1, vec2(cos_theta, sin_theta)),
//       dot(v1, vec2(-sin_theta, cos_theta))
//    );
//    vec3 v3 = in_mat * vec3(v2 + center, 1.0);
//
//	v_texcoord = vec3(in_texRect.x + texcoord.x * in_texRect.z, in_texRect.y + texcoord.y * in_texRect.w, texidx);
//	v_color = in_colorRGBA;
//    gl_Position = vec4(v3.x, v3.y, z, 1.0);
//}
//)--";
//
//// todo: 贴图数组
//inline static decltype(auto) fsSrc = R"--(#version 300 es
//precision mediump float;
//
//uniform sampler2D u_sampler;
//
//in vec3 v_texcoord;
//in vec4 v_color;
//
//out vec4 out_color;
//
//void main() {
//	out_color = texture(u_sampler, v_texcoord.xy) * v_color;
//}
//)--";
//
//namespace GameLogic {
//	struct Scene;
//}
//
//struct Game : xx::es::Context<Game> {
//	static Game& Instance() { return *(Game*)instance; }
//
//	xx::es::Shader vs, fs;
//	xx::es::Program ps;
//	xx::es::VertexArrays vao;
//	xx::es::Buffer vbo1, vbo2;
//	xx::es::Texture tex;
//	GLuint u_sampler{}, in_index{}, in_mat{}, in_posXYZtextureIndex{}, in_texRect{}, in_colorRGBA{}, in_scaleXYangleZ{};
//
//	// 个数上限. Run 前可改
//	GLuint m_max_quad_count = 1000000;
//
//	// projection 矩阵( 在 GLInit 中初始化 )
//	Mat matProj;
//
//	// todo: 贴图下标分配器
//
//	// 逻辑场景
//	xx::Shared<GameLogic::Scene> scene;
//
//	// 初始化
//	void Init(int const& w, int const& h, GLuint const& n, bool vsync);
//
//	// 逻辑更新
//	void Update();
//
//	// GL 初始化
//	int GLInit();
//
//	// GL 绘制
//	void Draw();
//};
//
//
//
//#include <xx_math.h>
//#include <xx_string.h>
//#include <xx_randoms.h>
//
//namespace GameLogic {
//	// 下列代码模拟 一组 圆形对象, 位于 2d 空间，一开始挤在一起，之后物理随机排开, 直到静止。统计一共要花多长时间。
//	// 数据结构方面不做特殊优化，走正常 oo 风格
//	// 要对比的是 用 或 不用 grid 系统的效率
//
//	// 坐标类型限定
//	using XY = xx::XY<int>;
//
//	// 模拟配置. 格子尺寸, grid 列数均为 2^n 方便位运算
//	static const int gridWidth = 128;
//	static const int gridHeight = 64;
//	static const int gridDiameter = 128;
//
//	static const int mapMinX = gridDiameter;							// >=
//	static const int mapMaxX = gridDiameter * (gridWidth - 1) - 1;		// <=
//	static const int mapMinY = gridDiameter;
//	static const int mapMaxY = gridDiameter * (gridHeight - 1) - 1;
//	static const XY mapCenter = { (mapMinX + mapMaxX) / 2, (mapMinY + mapMaxY) / 2 };
//
//	// Foo 的半径 / 直径 / 移动速度( 每帧移动单位距离 ) / 邻居查找个数
//	static const int fooRadius = 50;
//	static const int fooSpeed = 5;
//	static const int fooFindNeighborLimit = 20;	// 9格范围内通常能容纳的个数
//
//	struct Scene;
//	struct Foo {
//		// 当前半径
//		int16_t radius = fooRadius;
//		// 当前速度
//		int16_t speed = fooSpeed;
//		// 所在 grid 下标. 放入 im->grid 后，该值不为 -1
//		int gridIndex = -1;
//		// 当前坐标( Update2 单线程填充, 同时同步 grid )
//		XY xy;
//		// 即将生效的坐标( Update 多线并行填充 )
//		XY xy2;
//
//		// 构造时就放入 grid 系统
//		Foo(Scene* const& scene, XY const& xy);
//
//		// 计算下一步 xy 的动向, 写入 xy2
//		void Update(Scene* const& scene);
//
//		// 令 xy2 生效, 同步 grid
//		void Update2(Scene* const& scene);
//	};
//
//	struct Scene {
//		Scene() = delete;
//		Scene(Scene const&) = delete;
//		Scene& operator=(Scene const&) = delete;
//
//		// 格子系统( int16 则总 item 个数不能超过 64k, int 就敞开用了 )
//		xx::Grid2d<Foo*, int> grid;
//
//		// 对象容器 需要在 格子系统 的下方，确保 先 析构，这样才能正确的从 格子系统 移除
//		std::vector<Foo> objs;
//
//		// 随机数一枚, 用于对象完全重叠的情况下产生一个移动方向
//		xx::Random1 rnd;
//
//		// 每帧统计还有多少个对象正在移动
//		int count = 0;
//
//		Scene(int const& fooCount) {
//			// 初始化 2d 空间索引 容器
//			grid.Init(gridHeight, gridWidth, fooCount);
//
//			// 必须先预留足够多个数, 否则会导致数组变化, 指针失效
//			objs.reserve(fooCount);
//
//			// 直接构造出 n 个 Foo
//			for (int i = 0; i < fooCount; i++) {
//				objs.emplace_back(this, mapCenter);
//			}
//		}
//
//		void Update() {
//			count = 0;
//			auto siz = objs.size();
//			// 这里先简化，没有删除行为。否则就要记录到线程安全队列，事后批量删除
//#ifdef NDEBUG
//#pragma omp parallel for
//#endif
//			for (int i = 0; i < siz; ++i) {
//				objs[i].Update(this);
//			}
//			for (int i = 0; i < siz; ++i) {
//				objs[i].Update2(this);
//			}
//		}
//	};
//
//	inline Foo::Foo(Scene* const& scene, XY const& xy) {
//		this->xy = xy;
//		gridIndex = scene->grid.Add(xy.y / gridDiameter, xy.x / gridDiameter, this);
//	}
//
//	inline void Foo::Update(Scene* const& scene) {
//		// 提取坐标
//		auto xy = this->xy;
//
//		// 判断周围是否有别的 Foo 和自己重叠，取前 n 个，根据对方和自己的角度，结合距离，得到 推力矢量，求和 得到自己的前进方向和速度。
//		// 最关键的是最终移动方向。速度需要限定最大值和最小值。如果算出来矢量为 0，那就随机角度正常速度移动。
//		// 移动后的 xy 如果超出了 地图边缘，就硬调整. 
//
//		// 查找 n 个邻居
//		int limit = fooFindNeighborLimit;
//		int crossNum = 0;
//		XY v;
//		scene->grid.LimitFindNeighbor(limit, gridIndex, [&](auto o) {
//			assert(o != this);
//			// 类型转换下方便使用
//			auto& f = *o;
//			// 如果圆心完全重叠，则不产生推力
//			if (f.xy == this->xy) {
//				++crossNum;
//				return;
//			}
//			// 准备判断是否有重叠. r1* r1 + r2 * r2 > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
//			auto r1 = (int)f.radius;
//			auto r2 = (int)this->radius;
//			auto r12 = r1 * r1 + r2 * r2;
//			auto p1 = f.xy;
//			auto p2 = this->xy;
//			auto p12 = (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
//			// 有重叠
//			if (r12 > p12) {
//				// 计算 f 到 this 的角度( 对方产生的推力方向 ). 
//				auto a = xx::GetAngle<(gridDiameter * 2 >= 1024)>(p1, p2);
//				// 重叠的越多，推力越大?
//				auto inc = xx::Rotate(XY{ this->speed * r12 / p12, 0 }, a);
//				v += inc;
//				++crossNum;
//			}
//			});
//
//		if (crossNum) {
//			// 如果 v == 0 那就随机角度正常速度移动
//			if (v.IsZero()) {
//				auto a = scene->rnd.Next() % xx::table_num_angles;
//				auto inc = xx::Rotate(XY{ speed, 0 }, a);
//				xy += inc;
//			}
//			// 根据 v 移动
//			else {
//				auto a = xx::GetAngleXY(v.x, v.y);
//				auto inc = xx::Rotate(XY{ speed * std::max(crossNum, 5), 0 }, a);
//				xy += inc;
//			}
//
//			// 边缘限定( 当前逻辑有重叠才移动，故边界检测放在这里 )
//			if (xy.x < mapMinX) xy.x = mapMinX;
//			else if (xy.x > mapMaxX) xy.x = mapMaxX;
//			if (xy.y < mapMinY) xy.y = mapMinY;
//			else if (xy.y > mapMaxY) xy.y = mapMaxY;
//		}
//
//		// 保存 xy ( update2 的时候应用 )
//		xy2 = xy;
//	}
//
//	inline void Foo::Update2(Scene* const& scene) {
//		// 变更检测与同步
//		if (xy2 != xy) {
//			++scene->count;
//			xy = xy2;
//			assert(gridIndex != -1);
//			scene->grid.Update(gridIndex, xy2.y / gridDiameter, xy2.x / gridDiameter);
//		}
//	}
//}
//
//void Game::Init(int const& w, int const& h, GLuint const& n, bool vsync) {
//	this->width = w;
//	this->height = h;
//	this->vsync = vsync ? 1 : 0;
//
//	assert(n > 0 && n <= m_max_quad_count);
//	scene.Emplace(n);
//}
//
//void Game::Update() {
//	//scene->Update();
//}
//
//inline void CreateOrthographicOffCenter(float const& left, float const& right, float const& bottom, float const& top, Mat& dst) {
//	assert(right != left);
//	assert(top != bottom);
//
//	dst.fill(0);
//	dst[0] = 2 / (right - left);
//	dst[4] = 2 / (top - bottom);
//
//	dst[6] = (left + right) / (left - right);
//	dst[7] = (top + bottom) / (bottom - top);
//	dst[8] = 1;
//}
//
//int Game::GLInit() {
//	// fill matrix
//	CreateOrthographicOffCenter(0, (float)width, 0, (float)height, matProj);
//
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	// todo: call from window resize event
//	//glViewport(0, 0, width, height);
//	//glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
//
//	// ps
//	vs = LoadVertexShader({ vsSrc }); if (!vs) return __LINE__;
//	fs = LoadFragmentShader({ fsSrc }); if (!fs) return __LINE__;
//	ps = LinkProgram(vs, fs); if (!ps) return __LINE__;
//
//	// ps args idxs
//	u_sampler = glGetUniformLocation(ps, "u_sampler");
//	in_index = glGetAttribLocation(ps, "in_index");
//	in_mat = glGetAttribLocation(ps, "in_mat");
//	in_posXYZtextureIndex = glGetAttribLocation(ps, "in_posXYZtextureIndex");
//	in_texRect = glGetAttribLocation(ps, "in_texRect");
//	in_colorRGBA = glGetAttribLocation(ps, "in_colorRGBA");
//	in_scaleXYangleZ = glGetAttribLocation(ps, "in_scaleXYangleZ");
//
//	// tex
//	tex = LoadEtc2TextureFile(rootPath / "b.pkm");
//
//	// vao
//	glGenVertexArrays(1, &vao.handle);
//	glBindVertexArray(vao);
//	{
//		// vbo instance
//		glGenBuffers(1, &vbo1.handle);
//		{
//			glBindBuffer(GL_ARRAY_BUFFER, vbo1);
//			glBufferData(GL_ARRAY_BUFFER, sizeof(QuadInstance) * m_max_quad_count, nullptr, GL_STREAM_DRAW);
//		}
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//		// vbo shared
//		glGenBuffers(1, &vbo2.handle);
//		{
//			glBindBuffer(GL_ARRAY_BUFFER, vbo2);
//			glBufferData(GL_ARRAY_BUFFER, sizeof(quadIndexs), quadIndexs, GL_STATIC_DRAW);
//		}
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//	}
//	glBindVertexArray(0);
//
//	check_gl_error();
//	return 0;
//}
//
//void Game::Draw() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glUseProgram(ps);
//	//glUniform4f(u_projection, 2.0f / width, 2.0f / height, -1.0f, -1.0f);
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, tex);
//	glUniform1i(u_sampler, 0);
//	check_gl_error();
//
//	glBindVertexArray(vao);
//	check_gl_error();
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
//	glVertexAttribPointer(in_index, 4, GL_FLOAT, GL_FALSE, sizeof(QuadIndex), 0);
//	glEnableVertexAttribArray(in_index);
//	check_gl_error();
//
//	auto numQuads = (GLsizei)scene->objs.size();
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
//	{
//		auto buf = (QuadInstance*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(QuadInstance) * numQuads,
//			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
//		auto foos = scene->objs.data();
//#ifdef NDEBUG
////#pragma omp parallel for
//#endif
//		for (int i = 0; i < numQuads; ++i) {
//			auto& b = buf[i];
//			auto const& f = foos[i];
//
//			b.mat = matProj;
//
//			b.posX = f.xy.x * 0.1225f;
//			b.posY = f.xy.y * 0.1225f;
//			b.posZ = 0;
//			b.textureIndex = 0;	// todo
//
//			b.textureRectX = 0;
//			b.textureRectY = 0;
//			b.textureRectWidth = 1.f;
//			b.textureRectHeight = 1.f;
//
//			b.colorR = 255;
//			b.colorG = 255;
//			b.colorB = 255;
//			b.colorA = 255;
//
//			b.scaleX = (15.f / GameLogic::fooRadius) * f.radius;
//			b.scaleY = b.scaleX;
//			b.angleZ = 0;
//		}
//	}
//	glUnmapBuffer(GL_ARRAY_BUFFER);
//	check_gl_error();
//
//	{
//		glVertexAttribPointer(in_mat + 0, 3, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), (GLvoid*)offsetof(QuadInstance, mat));
//		glVertexAttribPointer(in_mat + 1, 3, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), (GLvoid*)(offsetof(QuadInstance, mat) + sizeof(float) * 3));
//		glVertexAttribPointer(in_mat + 2, 3, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), (GLvoid*)(offsetof(QuadInstance, mat) + sizeof(float) * 6));
//		glVertexAttribDivisor(in_mat + 0, 1);
//		glVertexAttribDivisor(in_mat + 1, 1);
//		glVertexAttribDivisor(in_mat + 2, 1);
//		glEnableVertexAttribArray(in_mat + 0);
//		glEnableVertexAttribArray(in_mat + 1);
//		glEnableVertexAttribArray(in_mat + 2);
//	}
//	check_gl_error();
//
//	glVertexAttribPointer(in_posXYZtextureIndex, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), (GLvoid*)offsetof(QuadInstance, posX));
//	glVertexAttribDivisor(in_posXYZtextureIndex, 1);
//	glEnableVertexAttribArray(in_posXYZtextureIndex);
//	check_gl_error();
//
//	glVertexAttribPointer(in_texRect, 4, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), (GLvoid*)offsetof(QuadInstance, textureRectX));
//	glVertexAttribDivisor(in_texRect, 1);
//	glEnableVertexAttribArray(in_texRect);
//	check_gl_error();
//
//	glVertexAttribPointer(in_colorRGBA, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadInstance), (GLvoid*)offsetof(QuadInstance, colorR));
//	glVertexAttribDivisor(in_colorRGBA, 1);
//	glEnableVertexAttribArray(in_colorRGBA);
//	check_gl_error();
//
//	glVertexAttribPointer(in_scaleXYangleZ, 3, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), (GLvoid*)offsetof(QuadInstance, scaleX));
//	glVertexAttribDivisor(in_scaleXYangleZ, 1);
//	glEnableVertexAttribArray(in_scaleXYangleZ);
//	check_gl_error();
//
//	glDrawArraysInstanced(GL_TRIANGLES, 0, _countof(quadIndexs), numQuads);
//	check_gl_error();
//}
//
//int main() {
//#ifdef NDEBUG
//	// 拿一半的 cpu 跑物理( 对于超线程 cpu 来说这样挺好 )
//	auto numThreads = std::thread::hardware_concurrency();
//	omp_set_num_threads(numThreads / 2);
//	omp_set_dynamic(0);
//#endif
//
//	Game g;
//	g.Init(1920, 1080, 1000000, false);
//	if (int r = g.Run()) {
//		std::cout << "g.Run() r = " << r << ", lastErrorNumber = " << g.lastErrorNumber << ", lastErrorMessage = " << g.lastErrorMessage << std::endl;
//	}
//}






//
//#include "esBase.h"
//#include "xx_string.h"
//
//#ifndef NDEBUG
//void check_gl_error_at(const char* file, int line, const char* func) {
//	GLenum err = glGetError();
//	if (err != GL_NO_ERROR) {
//		printf("OpenGL error: 0x%04x, at %s:%d, %s\n", err, file, line, func);
//		abort();
//	}
//}
//
//#define check_gl_error() check_gl_error_at(__FILE__, __LINE__, __FUNCTION__)
//#else
//#define check_gl_error() ((void)0)
//#endif
//
//
//// 测试结论：性能 和 传输流量 呈 反比，和 显示尺寸 呈 反比
//// 显示尺寸无法控制, 传输流量尽量省。mvp 矩阵一旦放入 quad data, 即便不用, 性能也会减半
//
//struct QuadData_center_scale_rotate {
//	int16_t x;
//	int16_t y;
//
//	int16_t scale;
//	int16_t rotate;
//};
//
//struct QuadData_offset_texcoord {
//	uint16_t x;
//	uint16_t y;
//};
//
//inline static const QuadData_offset_texcoord offset_and_texcoord[4] = {
//{ 0, 0 },
//{ 0, 65535 },
//{ 65535, 0 },
//{ 65535, 65535 },
//};
//
//struct Game : xx::es::Context<Game> {
//	static Game& Instance() { return *(Game*)instance; }
//
//	inline static decltype(auto) vsSrc = R"--(#version 300 es
//uniform vec4 u_projection;
//in vec2 in_vert;
//in vec2 in_center;
//in vec2 in_scale_rotate;
//out vec2 v_texcoord;
//void main() {
//    vec2 offset = vec2(in_vert.x - 0.5f, in_vert.y - 0.5f);
//    vec2 texcoord = in_vert;
//    vec2 center = in_center;
//    float scale = in_scale_rotate.x;
//    float rotate = in_scale_rotate.y;
//
//    float cos_theta = cos(rotate);
//    float sin_theta = sin(rotate);
//    vec2 v1 = offset * scale;
//    vec2 v2 = vec2(
//       dot(v1, vec2(cos_theta, sin_theta)),
//       dot(v1, vec2(-sin_theta, cos_theta))
//       );
//    vec2 v3 = (v2 + center) * u_projection.xy + u_projection.zw;
//
//    v_texcoord = texcoord;
//    gl_Position = vec4(v3.x, v3.y, 0.0, 1.0);
//}
//)--";
//	inline static decltype(auto) fsSrc = R"--(#version 300 es
//precision mediump float;
//
//uniform sampler2D u_sampler;
//in vec2 v_texcoord;
//out vec4 out_color;
//void main() {
//	out_color = texture(u_sampler, v_texcoord);
//}
//)--";
//
//	xx::es::Shader vs, fs;
//	xx::es::Program ps;
//	xx::es::VertexArrays vao;
//	xx::es::Buffer vbo1, vbo2;
//	xx::es::Texture tex;
//	GLuint u_projection{}, u_sampler{}, in_vert{}, in_center{}, in_scale_rotate{};
//
//	// 个数上限
//	GLuint m_max_quad_count = 5000000;
//
//	// logic data. 通过 Init ( 个数 ) 填充
//	std::vector<QuadData_center_scale_rotate> quads;
//
//	inline int GLInit() {
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//		//glViewport(0, 0, width, height);		// todo: call from window resize event
//		//glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
//
//		// ps
//		vs = LoadVertexShader({ vsSrc }); if (!vs) return __LINE__;
//		fs = LoadFragmentShader({ fsSrc }); if (!fs) return __LINE__;
//		ps = LinkProgram(vs, fs); if (!ps) return __LINE__;
//
//
//		// ps args idxs
//		u_projection = glGetUniformLocation(ps, "u_projection");
//		u_sampler = glGetUniformLocation(ps, "u_sampler");
//
//		in_vert = glGetAttribLocation(ps, "in_vert");
//		in_center = glGetAttribLocation(ps, "in_center");
//		in_scale_rotate = glGetAttribLocation(ps, "in_scale_rotate");
//
//		// tex
//		tex = LoadEtc2TextureFile(rootPath / "b.pkm");
//
//		// vao
//		glGenVertexArrays(1, &vao.handle);
//		glBindVertexArray(vao);
//		{
//			// vbo instance
//			glGenBuffers(1, &vbo1.handle);
//			{
//				glBindBuffer(GL_ARRAY_BUFFER, vbo1);
//				glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData_center_scale_rotate) * m_max_quad_count, nullptr, GL_STREAM_DRAW);
//			}
//			glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//			// vbo shared
//			glGenBuffers(1, &vbo2.handle);
//			{
//				glBindBuffer(GL_ARRAY_BUFFER, vbo2);
//				glBufferData(GL_ARRAY_BUFFER, sizeof(offset_and_texcoord), offset_and_texcoord, GL_STATIC_DRAW);
//			}
//			glBindBuffer(GL_ARRAY_BUFFER, 0);
//		}
//		glBindVertexArray(0);
//
//		check_gl_error();
//		return 0;
//	}
//
//	void Update() {
//		// todo: change quads logic here
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo1);
//		{
//			auto buf = (QuadData_center_scale_rotate*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(QuadData_center_scale_rotate) * quads.size(),
//				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
//			memcpy(buf, quads.data(), sizeof(QuadData_center_scale_rotate) * quads.size());
//			glUnmapBuffer(GL_ARRAY_BUFFER);
//		}
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		check_gl_error();
//	}
//
//	void Draw() {
//		glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);
//
//		glUseProgram(ps);
//		glUniform4f(u_projection, 2.0f / width, 2.0f / height, -1.0f, -1.0f);
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, tex);
//		glUniform1i(u_sampler, 0);
//		check_gl_error();
//
//		glBindVertexArray(vao);
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo2);
//		glVertexAttribPointer(in_vert, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(QuadData_offset_texcoord), 0);
//		glEnableVertexAttribArray(in_vert);
//		check_gl_error();
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo1);
//
//		glVertexAttribPointer(in_center, 2, GL_SHORT, GL_FALSE, sizeof(QuadData_center_scale_rotate), (GLvoid*)offsetof(QuadData_center_scale_rotate, x));
//		glVertexAttribDivisor(in_center, 1);
//		glEnableVertexAttribArray(in_center);
//
//		glVertexAttribPointer(in_scale_rotate, 2, GL_SHORT, GL_FALSE, sizeof(QuadData_center_scale_rotate), (GLvoid*)offsetof(QuadData_center_scale_rotate, scale));
//		glVertexAttribDivisor(in_scale_rotate, 1);
//		glEnableVertexAttribArray(in_scale_rotate);
//
//		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, _countof(offset_and_texcoord), (GLsizei)quads.size());
//		check_gl_error();
//	}
//
//	void Init(int const& w, int const& h, GLuint const& n, bool vsync) {
//		assert(n > 0 && n <= m_max_quad_count);
//		this->width = w;
//		this->height = h;
//		this->vsync = vsync ? 1 : 0;
//
//		auto random_of_range = [](GLfloat min, GLfloat max) {
//			return min + (max - min) * (GLfloat(rand()) / RAND_MAX);
//		};
//
//		quads.resize(n);
//		for (auto& q : quads) {
//			q.x = random_of_range(0, w);
//			q.y = random_of_range(0, h);
//			q.scale = 64;// 128;
//			q.rotate = 0;
//		}
//	}
//};
//
//int main() {
//	Game g;
//	g.Init(1920, 1080, 1000000, false);
//	if (int r = g.Run()) {
//		std::cout << "g.Run() r = " << r << ", lastErrorNumber = " << g.lastErrorNumber << ", lastErrorMessage = " << g.lastErrorMessage << std::endl;
//	}
//}














#include "xx_ptr.h"
#include "glfw/glfw3.h"
#include "glad/glad.h"

#include "linmath.h"
#ifdef _MSC_VER
#undef inline
#endif

#define var decltype(auto)

//#include "../gles3test1/gltest1.hpp"
#include "../gles3test1/gltest2.hpp"
//#include "../gles3test1/gltest3.hpp"
//#include "../gles3test1/gltest4.hpp"
//#include "../gles3test1/gltest5.hpp"

#include <thread>

GLFWwindow* wnd = nullptr;
inline int width = 0;
inline int height = 0;

int main() {
	//#ifdef NDEBUG
	//// 拿一半的 cpu 跑物理( 对于超线程 cpu 来说这样挺好 )
	//auto numThreads = std::thread::hardware_concurrency();
	//omp_set_num_threads(numThreads / 2);
	//omp_set_dynamic(0);
	//#endif



	glfwSetErrorCallback([](int error, const char* description) {
		throw new std::exception(description, error);
		});

	if (!glfwInit()) return -1;
	auto sg_glfw = xx::MakeScopeGuard([] { glfwTerminate(); });

	wnd = glfwCreateWindow(1280, 720, "xx2dtest1", nullptr, nullptr);
	if (!wnd) return -2;
	auto sg_wnd = xx::MakeScopeGuard([&] { glfwDestroyWindow(wnd); });

	//glfwSetWindowUserPointer(wnd, ) for store wnd handler class pointer

	glfwSetKeyCallback(wnd, [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
		assert(wnd == ::wnd);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(wnd, GLFW_TRUE);
		}
		});

	glfwSetFramebufferSizeCallback(wnd, [](GLFWwindow* wnd, int width, int height) {
		assert(wnd == ::wnd);
		::width = width;
		::height = height;
		});
	glfwGetFramebufferSize(wnd, &width, &height);

	glfwMakeContextCurrent(wnd);
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) return -3;
	glfwSwapInterval(0);

	//GLTest1 gltest;
	GLTest2 gltest;
	//GLTest3 gltest;
	//GLTest4 gltest;
	//GLTest5 gltest;

	while (!glfwWindowShouldClose(wnd)) {
		gltest.Update(width, height, (float)glfwGetTime());

		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	return 0;
}
