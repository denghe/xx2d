#include "pch.h"
#include <xx_file.h>
#include "logic.h"

#ifndef NDEBUG
inline void CheckGLErrorAt(const char* file, int line, const char* func) {
	if (auto e = glGetError(); e != GL_NO_ERROR) {
		throw std::runtime_error(xx::ToStringFormat("OpenGL error: {0}, at {1}:{2}, {3}", e, file, line, func));
	}
}
#define CheckGLError() CheckGLErrorAt(__FILE__, __LINE__, __FUNCTION__)
#else
#define CheckGLError() ((void)0)
#endif

enum class GLResTypes {
	Shader, Program, Vertexss, Buffer, Texture
};

template<GLResTypes T, typename...VS>
struct GLRes {
	std::tuple<GLuint, VS...> vs;
	operator GLuint const& () const { return std::get<0>(vs); }

	GLRes() = delete;
	GLRes(GLRes const&) = delete;
	GLRes& operator=(GLRes const&) = delete;

	GLRes(GLuint i) : vs(std::make_tuple(i)) {}
	template<typename...Args>
	GLRes(GLuint i, Args&&... args) : vs(std::make_tuple(i, std::forward<Args>(args)...)) {}

	~GLRes() {
		if constexpr (T == GLResTypes::Shader) { glDeleteShader(std::get<0>(vs)); }
		if constexpr (T == GLResTypes::Program) { glDeleteProgram(std::get<0>(vs)); }
		if constexpr (T == GLResTypes::Vertexss) { glDeleteVertexArrays(1, &std::get<0>(vs)); }
		if constexpr (T == GLResTypes::Buffer) { glDeleteBuffers(1, &std::get<0>(vs)); }
		if constexpr (T == GLResTypes::Texture) { glDeleteTextures(1, &std::get<0>(vs)); }
	}
};
using Shader = GLRes<GLResTypes::Shader>;
using Program = GLRes<GLResTypes::Program>;
using VertexArrays = GLRes<GLResTypes::Vertexss>;
using Buffer = GLRes<GLResTypes::Buffer>;
using Texture = GLRes<GLResTypes::Texture, GLsizei, GLsizei>;

xx::Shared<Texture> MakeTexture(std::string_view const& fn) {
	xx::Data d;
	if (int r = xx::ReadAllBytes(fn, d)) throw std::logic_error(xx::ToString("read file error. r = ", r, ", fn = ", fn));
	if (d.len >= 6 && memcmp("PKM 20", d.buf, 6) == 0 && d.len >= 16) {
		auto p = (uint8_t*)d.buf;
		uint16_t format = (p[6] << 8) | p[7];				// 格式值:  1 ETC2_RGB_NO_MIPMAPS, 3 ETC2_RGBA_NO_MIPMAPS
		uint16_t encodedWidth = (p[8] << 8) | p[9];			// 4 字节对齐 宽
		uint16_t encodedHeight = (p[10] << 8) | p[11];		// 4 字节对齐 高
		uint16_t width = (p[12] << 8) | p[13];				// 宽
		uint16_t height = (p[14] << 8) | p[15];				// 高
		assert((format == 1 || format == 3) && width > 0 && height > 0 && encodedWidth >= width && encodedWidth - width < 4
			&& encodedHeight >= height && encodedHeight - height < 4 && d.len == 16 + encodedWidth * encodedHeight);
		GLuint t = 0;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// 这个似乎可根据 pixelsWide * bitsPerPixel / 8 的对齐情况 的值来填充 8, 4, 2, 1 ? 参看 cocos2.x CCTexture2D.cpp  line 191
		glGenTextures(1, &t);
		glActiveTexture(GL_TEXTURE0/* + textureUnit*/);
		glBindTexture(GL_TEXTURE_2D, t);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST/*GL_LINEAR*/);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST/*GL_LINEAR*/);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, format == 3 ? GL_COMPRESSED_RGBA8_ETC2_EAC : GL_COMPRESSED_RGB8_ETC2, (GLsizei)width, (GLsizei)height, 0, (GLsizei)(d.len - 16), p + 16);
		CheckGLError();
		return xx::Make<Texture>(t, width, height);
	}
	throw std::logic_error(xx::ToString("unsupported texture type. fn = ", fn));
}

union Color4b {
	struct {
		uint8_t r, g, b, a;
	};
	uint32_t data = 0;
};

void Logic::Update() {
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
