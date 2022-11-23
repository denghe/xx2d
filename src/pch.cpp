#include "pch.h"

// some opengl documents put here

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

已知问题：如果启用深度检测，则 alpha 不好使。除非 shader 里判断阈值并 discard, 但是那样效果不理想，仅限于做部分种类游戏0
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_ALWAYS);//glDepthFunc(GL_LEQUAL);
glDepthMask(GL_TRUE);

*/






//#include "pch.h"
//#include "logic.h"
//
//inline static auto vsSrc = R"(#version 300 es
//precision highp float;
//
//in vec4 aColor;
//in vec4 aPos;
//
//out mediump vec4 vColor;
//
//void main() {
//	vColor = aColor;
//	gl_Position = aPos;
//})"sv;
//
//inline static auto fsSrc = R"(#version 300 es
//precision mediump float;
//
//in vec4 vColor;
//
//out vec4 oColor;
//
//void main() {
//	oColor = vColor;
//})"sv;
//
//inline static std::array<GLfloat, 12> color = {
//	1.0f, 0.0f, 0.0f, 1.0f,
//	0.0f, 1.0f, 0.0f, 1.0f,
//	0.0f, 0.0f, 1.0f, 1.0f,
//};
//inline static std::array<GLfloat, 9> verts = {
//	0.0f,   0.5f,  0.0f,
//	-0.5f, -0.5f,  0.0f,
//	0.5f,  -0.5f,  0.0f
//};

//void Logic::GLInit() {
//	v = LoadVertexShader({ vsSrc });
//	f = LoadFragmentShader({ fsSrc });
//	p = LinkProgram(v, f);
//
//	uMVPMatrix = glGetUniformLocation(p, "uMVPMatrix");
//	uTex0 = glGetUniformLocation(p, "uTex0");
//
//	aColor = glGetAttribLocation(p, "aColor");
//	aPos = glGetAttribLocation(p, "aPos");
//	aTexCoord = glGetAttribLocation(p, "aTexCoord");
//
//	//glEnable(GL_DEPTH_TEST);
//	//glDepthFunc(GL_LEQUAL);
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//}
//
//void Logic::Update(float delta) {
//	assert(w >= 0 && h >= 0);
//	glViewport(0, 0, w, h);
//	glClear(GL_COLOR_BUFFER_BIT);
//	glUseProgram(p);														CheckGLError();
//	//glVertexAttrib4fv(aColor, color.data());								CheckGLError();
//	glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, 0, color.data());	CheckGLError();
//	glEnableVertexAttribArray(aColor);										CheckGLError();
//	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, verts.data());	CheckGLError();
//	glEnableVertexAttribArray(aPos);										CheckGLError();
//	glDrawArrays(GL_TRIANGLES, 0, 3);										CheckGLError();
//}






//
//#include "pch.h"
//#include "logic.h"
//
//inline static auto vsSrc = R"(#version 300 es
//precision highp float;
////uniform mat4 uMVPMatrix;
//
//in vec4 aColor;
//in vec4 aPos;
//in vec2 aTexCoord;
//
//out mediump vec4 vColor;
//out mediump vec2 vTexCoord;
//
//void main() {
//	vColor = aColor;
//	gl_Position = /*uMVPMatrix * */aPos;
//	vTexCoord = aTexCoord;
//})"sv;
//
//inline static auto fsSrc = R"(#version 300 es
//precision mediump float;
//uniform sampler2D uTex0;
//
//in vec4 vColor;
//in vec2 vTexCoord;
//
//out vec4 oColor;
//
//void main() {
//	oColor = vColor * texture(uTex0, vTexCoord);
//})"sv;
//
//inline static std::array<GLfloat, 4> color = {
//	1.0f, 1.0f, 1.0f, 1.0f,
//};
//inline static std::array<GLfloat, 20> verts = {
//	-0.5f,  -0.5f,  0.0f,/*     */0.0f, 1.0f,
//	-0.5f,   0.5f,  0.0f,/*     */0.0f, 0.0f,
//	 0.5f,   0.5f,  0.0f,/*     */1.0f, 0.0f,
//	 0.5f,  -0.5f,  0.0f,/*     */1.0f, 1.0f,
//};
//
//void Logic::GLInit() {
//	t = LoadTexture(XX_STRINGIFY(RES_ROOT_DIR)"/res/zazaka.pkm");
//
//	v = LoadVertexShader({ vsSrc });
//	f = LoadFragmentShader({ fsSrc });
//	p = LinkProgram(v, f);
//
//	//uMVPMatrix = glGetUniformLocation(p, "uMVPMatrix");
//	uTex0 = glGetUniformLocation(p, "uTex0");
//
//	aColor = glGetAttribLocation(p, "aColor");
//	aPos = glGetAttribLocation(p, "aPos");
//	aTexCoord = glGetAttribLocation(p, "aTexCoord");
//
//	glGenBuffers(1, &b.Ref());
//	glBindBuffer(GL_ARRAY_BUFFER, b);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts.size(), nullptr, GL_STREAM_DRAW);
//	//auto buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * verts.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);	// | GL_MAP_UNSYNCHRONIZED_BIT
//	//memcpy(buf, verts.data(), sizeof(float) * verts.size());
//	//glUnmapBuffer(GL_ARRAY_BUFFER);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glDisable(GL_CULL_FACE);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//}
//
//void Logic::Update(float delta) {
//	assert(w >= 0 && h >= 0);
//	glViewport(0, 0, w, h);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	glUseProgram(p);																							CheckGLError();
//
//	glVertexAttrib4fv(aColor, color.data());																	CheckGLError();
//
//	glBindBuffer(GL_ARRAY_BUFFER, b);
//	auto buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * verts.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);	// | GL_MAP_UNSYNCHRONIZED_BIT
//	memcpy(buf, verts.data(), sizeof(float) * verts.size());
//	glUnmapBuffer(GL_ARRAY_BUFFER);
//
//	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);									CheckGLError();
//	glEnableVertexAttribArray(aPos);																			CheckGLError();
//
//	glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));	CheckGLError();
//	glEnableVertexAttribArray(aTexCoord);																		CheckGLError();
//
//	glActiveTexture(GL_TEXTURE0);																				CheckGLError();
//	glBindTexture(GL_TEXTURE_2D, t);																			CheckGLError();
//	glUniform1i(uTex0, 0);																						CheckGLError();
//
//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);																		CheckGLError();
//}






//
//#include "pch.h"
//#include "logic.h"
//
//inline static auto vsSrc = R"(#version 300 es
//precision highp float;
//
//in vec4 aPos;
//
//out mediump vec2 vTexCoord;
//
//void main() {
//	gl_Position = vec4(aPos.xy, 0, 1);
//	vTexCoord = aPos.zw;
//})"sv;
//
//inline static auto fsSrc = R"(#version 300 es
//precision mediump float;
//uniform vec4 uColor;
//uniform sampler2D uTex0;
//
//in vec2 vTexCoord;
//
//out vec4 oColor;
//
//void main() {
//	oColor = uColor * texture(uTex0, vTexCoord);
//})"sv;
//
//struct RGBA8 {
//	uint8_t r, g, b, a;
//};
//inline static RGBA8 color = {
//	111, 111, 255, 255
//};
//
//struct XYUV {
//	float x, y, u, v;
//};
//inline static std::array<XYUV, 4> verts = {
//	-0.5f,  -0.5f,/*     */0.0f, 1.0f,
//	-0.5f,   0.5f,/*     */0.0f, 0.0f,
//	 0.5f,   0.5f,/*     */1.0f, 0.0f,
//	 0.5f,  -0.5f,/*     */1.0f, 1.0f,
//};
//
//void Logic::GLInit() {
//	t = LoadTexture(XX_STRINGIFY(RES_ROOT_DIR)"/res/zazaka.pkm");
//
//	v = LoadVertexShader({ vsSrc });
//	f = LoadFragmentShader({ fsSrc });
//	p = LinkProgram(v, f);
//
//	uColor = glGetUniformLocation(p, "uColor");
//	uTex0 = glGetUniformLocation(p, "uTex0");
//
//	aPos = glGetAttribLocation(p, "aPos");
//
//	glGenBuffers(1, &b.Ref());
//	glBindBuffer(GL_ARRAY_BUFFER, b);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), nullptr, GL_STREAM_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glDisable(GL_CULL_FACE);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//}
//
//void Logic::Update(float delta) {
//	assert(w >= 0 && h >= 0);
//	glViewport(0, 0, w, h);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	glUseProgram(p);																								CheckGLError();
//
//	glBindBuffer(GL_ARRAY_BUFFER, b);
//	auto buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(verts), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);	// | GL_MAP_UNSYNCHRONIZED_BIT
//	memcpy(buf, verts.data(), sizeof(verts));
//	glUnmapBuffer(GL_ARRAY_BUFFER);
//
//	glVertexAttribPointer(aPos, 4, GL_FLOAT, GL_FALSE, sizeof(XYUV), 0);											CheckGLError();
//	glEnableVertexAttribArray(aPos);																				CheckGLError();
//
//	glActiveTexture(GL_TEXTURE0);																					CheckGLError();
//	glBindTexture(GL_TEXTURE_2D, t);																				CheckGLError();
//	glUniform1i(uTex0, 0);																							CheckGLError();
//
//	glUniform4f(uColor, float(color.r) / 255, float(color.g) / 255, float(color.b) / 255, float(color.a) / 255);	CheckGLError();
//
//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);																			CheckGLError();
//}







//
//#include "pch.h"
//#include "logic.h"
//
//inline static auto vsSrc = R"(#version 300 es
//precision highp float;
//
//in vec2 aPos;
//in vec2 aTexCoord;
//in vec4 aColor;
//
//out mediump vec4 vColor;
//out mediump vec2 vTexCoord;
//
//void main() {
//	gl_Position = vec4(aPos, 0, 1);
//	vTexCoord = aTexCoord;
//	vColor = aColor;
//})"sv;
//
//inline static auto fsSrc = R"(#version 300 es
//precision mediump float;
//uniform sampler2D uTex0;
//
//in vec4 vColor;
//in vec2 vTexCoord;
//
//out vec4 oColor;
//
//void main() {
//	oColor = vColor * texture(uTex0, vTexCoord);
//})"sv;
//
//struct XY {
//	float x, y;
//};
//struct UV {
//	uint16_t u, v;
//};
//struct RGBA8 {
//	uint8_t r, g, b, a;
//};
//struct XYUVRGBA8 : XY, UV, RGBA8 {};
//
//inline static std::array<XYUVRGBA8, 4> verts = {
//	-0.5f,  -0.5f		, 0    , 65535		, 111, 111, 255, 255,
//	-0.5f,   0.5f		, 0    , 0			, 111, 111, 255, 255,
//	 0.5f,   0.5f		, 65535, 0			, 111, 111, 255, 255,
//	 0.5f,  -0.5f		, 65535, 65535		, 111, 111, 255, 255,
//};
//
//void Logic::GLInit() {
//	t = LoadTexture(XX_STRINGIFY(RES_ROOT_DIR)"/res/zazaka.pkm");
//
//	v = LoadVertexShader({ vsSrc });
//	f = LoadFragmentShader({ fsSrc });
//	p = LinkProgram(v, f);
//
//	uTex0 = glGetUniformLocation(p, "uTex0");
//
//	aPos = glGetAttribLocation(p, "aPos");
//	aTexCoord = glGetAttribLocation(p, "aTexCoord");
//	aColor = glGetAttribLocation(p, "aColor");
//
//	glGenBuffers(1, &b.Ref());
//	glBindBuffer(GL_ARRAY_BUFFER, b);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), nullptr, GL_STREAM_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glDisable(GL_CULL_FACE);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//}
//
//void Logic::Update(float delta) {
//	assert(w >= 0 && h >= 0);
//	glViewport(0, 0, w, h);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	glUseProgram(p);																								CheckGLError();
//
//	glBindBuffer(GL_ARRAY_BUFFER, b);
//	auto buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(verts), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);	// | GL_MAP_UNSYNCHRONIZED_BIT
//	memcpy(buf, verts.data(), sizeof(verts));
//	glUnmapBuffer(GL_ARRAY_BUFFER);
//
//	glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYUVRGBA8), 0);
//	glEnableVertexAttribArray(aPos);																				CheckGLError();
//	glVertexAttribPointer(aTexCoord, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, u));
//	glEnableVertexAttribArray(aTexCoord);																			CheckGLError();
//	glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, r));
//	glEnableVertexAttribArray(aColor);																				CheckGLError();
//
//	glActiveTexture(GL_TEXTURE0);																					CheckGLError();
//	glBindTexture(GL_TEXTURE_2D, t);																				CheckGLError();
//	glUniform1i(uTex0, 0);																							CheckGLError();
//
//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);																			CheckGLError();
//}





//
//#include "pch.h"
//#include "logic.h"
//
//inline static auto vsSrc = R"(#version 300 es
//precision highp float;
//uniform vec2 uCxy;	// center x y
//
//in vec2 aPos;
//in vec2 aTexCoord;
//in vec4 aColor;
//
//out mediump vec4 vColor;
//out mediump vec2 vTexCoord;
//
//void main() {
//	gl_Position = vec4(aPos / uCxy, 0, 1);
//	vTexCoord = aTexCoord;
//	vColor = aColor;
//})"sv;
//
//inline static auto fsSrc = R"(#version 300 es
//precision mediump float;
//uniform sampler2D uTex0;
//
//in vec4 vColor;
//in vec2 vTexCoord;
//
//out vec4 oColor;
//
//void main() {
//	oColor = vColor * texture(uTex0, vTexCoord);
//})"sv;
//
//struct XY {
//	float x, y;
//};
//struct UV {
//	uint16_t u, v;
//};
//struct RGBA8 {
//	uint8_t r, g, b, a;
//};
//struct XYUVRGBA8 : XY, UV, RGBA8 {};
//
//inline static std::array<XYUVRGBA8, 4> verts = {
//	-320.f,  -180.f		, 0    , 65535		, 111, 111, 255, 255,
//	-320.f,   180.f		, 0    , 0			, 111, 111, 255, 255,
//	 320.f,   180.f		, 65535, 0			, 111, 111, 255, 255,
//	 320.f,  -180.f		, 65535, 65535		, 111, 111, 255, 255,
//};
//
//void Logic::GLInit() {
//	t = LoadTexture(XX_STRINGIFY(RES_ROOT_DIR)"/res/zazaka.pkm");
//
//	v = LoadVertexShader({ vsSrc });
//	f = LoadFragmentShader({ fsSrc });
//	p = LinkProgram(v, f);
//
//	uCxy = glGetUniformLocation(p, "uCxy");
//	uTex0 = glGetUniformLocation(p, "uTex0");
//
//	aPos = glGetAttribLocation(p, "aPos");
//	aTexCoord = glGetAttribLocation(p, "aTexCoord");
//	aColor = glGetAttribLocation(p, "aColor");
//
//	glGenBuffers(1, &b.Ref());
//	glBindBuffer(GL_ARRAY_BUFFER, b);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), nullptr, GL_STREAM_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glDisable(GL_CULL_FACE);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//}
//
//void Logic::Update(float delta) {
//	assert(w >= 0 && h >= 0);
//	glViewport(0, 0, w, h);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	glUseProgram(p);																										CheckGLError();
//
//	glBindBuffer(GL_ARRAY_BUFFER, b);
//	auto buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(verts), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);	// | GL_MAP_UNSYNCHRONIZED_BIT
//	memcpy(buf, verts.data(), sizeof(verts));
//	glUnmapBuffer(GL_ARRAY_BUFFER);
//
//	glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYUVRGBA8), 0);
//	glEnableVertexAttribArray(aPos);																						CheckGLError();
//	glVertexAttribPointer(aTexCoord, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, u));
//	glEnableVertexAttribArray(aTexCoord);																					CheckGLError();
//	glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, r));
//	glEnableVertexAttribArray(aColor);																						CheckGLError();
//
//	glUniform2f(uCxy, w / 2, h / 2);
//
//	glActiveTexture(GL_TEXTURE0);																							CheckGLError();
//	glBindTexture(GL_TEXTURE_2D, t);																						CheckGLError();
//	glUniform1i(uTex0, 0);																									CheckGLError();
//
//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);																					CheckGLError();
//}





//
//
//    // reference from https://github.com/cslarsen/mersenne-twister
//    // faster than std impl, can store & restore state data directly
//    // ser/de data size == 5000 bytes
//struct Random5 : RandomBase<Random5> {
//    inline static const size_t SIZE = 624;
//    inline static const size_t PERIOD = 397;
//    inline static const size_t DIFF = SIZE - PERIOD;
//    inline static const uint32_t MAGIC = 0x9908b0df;
//    uint32_t MT[SIZE];
//    uint32_t MT_TEMPERED[SIZE];
//    size_t index = SIZE;
//    uint32_t seed;
//    void Reset(uint32_t const& seed = 1234567890) {
//        this->seed = seed;
//        MT[0] = seed;
//        index = SIZE;
//        for (uint_fast32_t i = 1; i < SIZE; ++i) {
//            MT[i] = 0x6c078965 * (MT[i - 1] ^ MT[i - 1] >> 30) + i;
//        }
//    }
//    Random5(uint32_t const& seed = 1234567890) {
//        Reset(seed);
//    }
//
//    Random5& operator=(Random5 const&) = default;
//    Random5(Random5&& o) = default;
//    Random5& operator=(Random5&& o) = default;
//
//#define Random5_M32(x) (0x80000000 & x) // 32nd MSB
//#define Random5_L31(x) (0x7FFFFFFF & x) // 31 LSBs
//#define Random5_UNROLL(expr) \
//  y = Random5_M32(MT[i]) | Random5_L31(MT[i+1]); \
//  MT[i] = MT[expr] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & MAGIC); \
//  ++i;
//    void Generate() {
//        size_t i = 0;
//        uint32_t y;
//        while (i < DIFF) {
//            Random5_UNROLL(i + PERIOD);
//        }
//        while (i < SIZE - 1) {
//            Random5_UNROLL(i - DIFF);
//        }
//        {
//            y = Random5_M32(MT[SIZE - 1]) | Random5_L31(MT[0]);
//            MT[SIZE - 1] = MT[PERIOD - 1] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & MAGIC);
//        }
//        for (size_t i = 0; i < SIZE; ++i) {
//            y = MT[i];
//            y ^= y >> 11;
//            y ^= y << 7 & 0x9d2c5680;
//            y ^= y << 15 & 0xefc60000;
//            y ^= y >> 18;
//            MT_TEMPERED[i] = y;
//        }
//        index = 0;
//    }
//#undef Random5_UNROLL
//#undef Random5_L31
//#undef Random5_M32
//
//    uint32_t Next() {
//        if (index == SIZE) {
//            Generate();
//            index = 0;
//        }
//        return MT_TEMPERED[index++];
//    }
//};
