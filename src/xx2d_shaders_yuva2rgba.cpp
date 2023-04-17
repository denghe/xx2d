#include "xx2d.h"

namespace xx {

	//void Shader_Yuva2Rgba::Init() {
	//	if (!engine.sm.shaders[index]) {
	//		auto s = Make<Shader_Yuva2Rgba>();
	//		engine.sm.shaders[index] = s;
	//		s->Init(&engine.sm);
	//	}
	//}

	void Shader_Yuva2Rgba::Init(ShaderManager* sm_) {
		sm = sm_;

		v = LoadGLVertexShader({ R"(#version 300 es
precision highp float;
uniform vec2 uCxy;	// screen center coordinate
uniform vec2 uStrideHeight;

in vec2 aPos;
in vec2 aTexCoord;	// pixel pos

out vec2 vTexCoord;	// 0 ~ 1

void main() {
	gl_Position = vec4(aPos * uCxy, 0, 1);
	vTexCoord = aTexCoord / uStrideHeight;
})"sv });

		f = LoadGLFragmentShader({ R"(#version 300 es
precision highp float;

uniform sampler2D uTexY;
uniform sampler2D uTexU;
uniform sampler2D uTexV;
uniform sampler2D uTexA;

in vec2 vTexCoord;

out vec4 oColor;

void main() {
	float y = texture( uTexY, vTexCoord ).r;
	float u = texture( uTexU, vTexCoord ).r - 0.5f;
	float v = texture( uTexV, vTexCoord ).r - 0.5f;
	float a = texture( uTexA, vTexCoord ).r;

	y = 1.1643f * (y - 0.0625f);
	float r = y + 1.5958f * v;
	float g = y - 0.39173f * u - 0.81290f * v;
	float b = y + 2.017f * u;

    oColor = vec4(r, g, b, a);
})"sv });

		p = LinkGLProgram(v, f);

		uCxy = glGetUniformLocation(p, "uCxy");
		uStrideHeight = glGetUniformLocation(p, "uStrideHeight");
		uTexY = glGetUniformLocation(p, "uTexY");
		uTexU = glGetUniformLocation(p, "uTexU");
		uTexV = glGetUniformLocation(p, "uTexV");
		uTexA = glGetUniformLocation(p, "uTexA");

		aPos = glGetAttribLocation(p, "aPos");
		aTexCoord = glGetAttribLocation(p, "aTexCoord");
		CheckGLError();

		glGenVertexArrays(1, va.Get());
		glBindVertexArray(va);
		glGenBuffers(1, (GLuint*)&vb);
		glGenBuffers(1, (GLuint*)&ib);

		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYUV), 0);
		glEnableVertexAttribArray(aPos);
		glVertexAttribPointer(aTexCoord, 2, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(XYUV), (GLvoid*)offsetof(XYUV, u));
		glEnableVertexAttribArray(aTexCoord);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		GLushort idxs[6] = { 0, 1, 2, 0, 2, 3 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idxs), idxs, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CheckGLError();
	}

	void Shader_Yuva2Rgba::Begin() {
		if (sm->cursor != index) {
			// here can check shader type for combine batch
			sm->shaders[sm->cursor]->End();
			sm->cursor = index;

			engine.GLDisableBlend();
		}
	}

	void Shader_Yuva2Rgba::End() {
	}


	void Shader_Yuva2Rgba::Draw(uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData, uint32_t const& yaStride, uint32_t const& uvStride, uint32_t const& w, uint32_t const& h, XY const& pos) {

		glUseProgram(p);
		glUniform2f(uCxy, 2 / engine.w, 2 / engine.h);
		glUniform2f(uStrideHeight, yaStride, h);
		glUniform1i(uTexY, 0);
		glUniform1i(uTexU, 1);
		glUniform1i(uTexV, 2);
		glUniform1i(uTexA, 3);

		glBindVertexArray(va);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

		XYUV xyuv[4];

		auto w2 = w / 2;
		auto h2 = h / 2;

		xyuv[0].x = pos.x - w2;
		xyuv[0].y = pos.y - h2;
		xyuv[0].u = 0;
		xyuv[0].v = 0;

		xyuv[1].x = pos.x - w2;
		xyuv[1].y = pos.y + h2;
		xyuv[1].u = 0;
		xyuv[1].v = h;

		xyuv[2].x = pos.x + w2;
		xyuv[2].y = pos.y + h2;
		xyuv[2].u = w;
		xyuv[2].v = h;

		xyuv[3].x = pos.x + w2;
		xyuv[3].y = pos.y - h2;
		xyuv[3].u = w;
		xyuv[3].v = 0;

#ifdef __EMSCRIPTEN__
		auto c = GL_LUMINANCE;
#else
		auto c = GL_RED;
#endif
		GLTextureCore texY{ LoadGLTexture_core(0) };
		glTexImage2D(GL_TEXTURE_2D, 0, c, yaStride, h, 0, c, GL_UNSIGNED_BYTE, yData);
		CheckGLError();
		

		GLTextureCore texU{ LoadGLTexture_core(1) };
		glTexImage2D(GL_TEXTURE_2D, 0, c, uvStride, h / 2, 0, c, GL_UNSIGNED_BYTE, uData);
		CheckGLError();

		GLTextureCore texV{ LoadGLTexture_core(2) };
		glTexImage2D(GL_TEXTURE_2D, 0, c, uvStride, h / 2, 0, c, GL_UNSIGNED_BYTE, vData);
		CheckGLError();

		GLTextureCore texA{ LoadGLTexture_core(3) };
		glTexImage2D(GL_TEXTURE_2D, 0, c, yaStride, h, 0, c, GL_UNSIGNED_BYTE, aData);
		CheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(xyuv), xyuv, GL_STREAM_DRAW);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, {});
		CheckGLError();

		sm->drawVerts += 6;
		sm->drawCall += 1;
	}
}

/*

inline static int Yuva2Rgba(std::vector<uint8_t>& bytes
	, uint32_t const& w, uint32_t const& h
	, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData
	, uint32_t const& yaStride, uint32_t const& uvStride) {

#ifdef LIBYUV_API
	bytes.resize(w * h * 4);
	return libyuv::I420AlphaToABGR(yData, yaStride, uData, uvStride, vData, uvStride, aData, yaStride, bytes.data(), w * 4, w, h, 0);
#else
	bytes.clear();
	bytes.reserve(w * h * 4);

	for (uint32_t _h = 0; _h < h; ++_h) {
		for (uint32_t _w = 0; _w < w; ++_w) {
			// calc index. 1 uv =  4 ya
			auto&& yaIdx = yaStride * _h + _w;
			auto&& uvIdx = uvStride * (_h / 2) + _w / 2;

			// byte -> float
			auto&& y = yData[yaIdx] / 255.0f;
			auto&& u = uData[uvIdx] / 255.0f;
			auto&& v = vData[uvIdx] / 255.0f;

			// calc
			y = 1.1643f * (y - 0.0625f);
			u = u - 0.5f;
			v = v - 0.5f;

			// yuv to float rgb
			auto&& r = y + 1.5958f * v;
			auto&& g = y - 0.39173f * u - 0.81290f * v;
			auto&& b = y + 2.017f * u;

			// cut to 0 ~ 1
			if (r > 1.0f) r = 1.0f; else if (r < 0.0f) r = 0.0f;
			if (g > 1.0f) g = 1.0f; else if (g < 0.0f) g = 0.0f;
			if (b > 1.0f) b = 1.0f; else if (b < 0.0f) b = 0.0f;

			// store
			bytes.push_back((uint8_t)(r * 255));
			bytes.push_back((uint8_t)(g * 255));
			bytes.push_back((uint8_t)(b * 255));
			bytes.push_back(aData ? aData[yaIdx] : (uint8_t)0);
		}
	}
	return 0;
#endif
}

*/