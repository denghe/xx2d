#pragma once
#include "xx_helpers.h"

#define GLAD_MALLOC(sz)       malloc(sz)
#define GLAD_FREE(ptr)        free(ptr)
#define GLAD_GL_IMPLEMENTATION
#include "glad.h"
#include "glfw3.h"

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
	GLuint& Ref() { return std::get<0>(vs); }

	GLRes(GLRes const&) = delete;
	GLRes& operator=(GLRes const&) = delete;
	GLRes() = default;
	GLRes(GLuint&& i) {
		std::get<0>(vs) = i;
	}
	GLRes(GLRes&& o) noexcept {
		std::swap(vs, o.vs);
	}
	GLRes& operator=(GLRes&& o) noexcept {
		std::swap(vs, o.vs);
		return *this;
	}

	GLRes(GLuint i) : vs(std::make_tuple(i)) {}
	template<typename...Args>
	GLRes(GLuint i, Args&&... args) : vs(std::make_tuple(i, std::forward<Args>(args)...)) {}

	~GLRes() {
		if (!std::get<0>(vs)) return;
		if constexpr (T == GLResTypes::Shader) { glDeleteShader(std::get<0>(vs)); }
		if constexpr (T == GLResTypes::Program) { glDeleteProgram(std::get<0>(vs)); }
		if constexpr (T == GLResTypes::Vertexss) { glDeleteVertexArrays(1, &std::get<0>(vs)); }
		if constexpr (T == GLResTypes::Buffer) { glDeleteBuffers(1, &std::get<0>(vs)); }
		if constexpr (T == GLResTypes::Texture) { glDeleteTextures(1, &std::get<0>(vs)); }
		std::get<0>(vs) = 0;
	}
};

using GLShader = GLRes<GLResTypes::Shader>;

using GLProgram = GLRes<GLResTypes::Program>;

using GLVertexArrays = GLRes<GLResTypes::Vertexss>;

using GLBuffer = GLRes<GLResTypes::Buffer>;

using GLTexture = GLRes<GLResTypes::Texture, GLsizei, GLsizei, std::string>;



inline GLTexture LoadGLTexture(std::string_view const& buf, std::string_view const& fullPath) {
	if (buf.size() >= 6 && memcmp("PKM 20", buf.data(), 6) == 0 && buf.size() >= 16) {
		auto p = (uint8_t*)buf.data();
		uint16_t format = (p[6] << 8) | p[7];				// 1 ETC2_RGB_NO_MIPMAPS, 3 ETC2_RGBA_NO_MIPMAPS
		uint16_t encodedWidth = (p[8] << 8) | p[9];			// 4 align width
		uint16_t encodedHeight = (p[10] << 8) | p[11];		// 4 align height
		uint16_t width = (p[12] << 8) | p[13];				// width
		uint16_t height = (p[14] << 8) | p[15];				// height
		assert((format == 1 || format == 3) && width > 0 && height > 0 && encodedWidth >= width && encodedWidth - width < 4
			&& encodedHeight >= height && encodedHeight - height < 4 && buf.size() == 16 + encodedWidth * encodedHeight);
		GLuint t = 0;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// todo: optimize this value like cocos2.x CCTexture2D.cpp  line 191
		glGenTextures(1, &t);
		glActiveTexture(GL_TEXTURE0/* + textureUnit*/);
		glBindTexture(GL_TEXTURE_2D, t);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST/*GL_LINEAR*/);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST/*GL_LINEAR*/);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, format == 3 ? GL_COMPRESSED_RGBA8_ETC2_EAC : GL_COMPRESSED_RGB8_ETC2, (GLsizei)width, (GLsizei)height, 0, (GLsizei)(buf.size() - 16), p + 16);
		CheckGLError();
		return { t, width, height, fullPath };
	}
	throw std::logic_error(xx::ToString("unsupported texture type. fn = ", fullPath));
}


inline GLShader LoadGLShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_) {
	assert(codes_.size() && (type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER));
	auto&& shader = glCreateShader(type);
	if (!shader)
		throw std::logic_error(xx::ToString("glCreateShader(", type, ") failed."));
	std::vector<GLchar const*> codes;
	codes.resize(codes_.size());
	std::vector<GLint> codeLens;
	codeLens.resize(codes_.size());
	auto ss = codes_.begin();
	for (size_t i = 0; i < codes.size(); ++i) {
		codes[i] = (GLchar const*)ss[i].data();
		codeLens[i] = (GLint)ss[i].size();
	}
	glShaderSource(shader, (GLsizei)codes_.size(), codes.data(), codeLens.data());
	glCompileShader(shader);
	GLint r = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
	if (!r) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &r);	// fill txt len into r
		std::string s;
		if (r) {
			s.resize(r);
			glGetShaderInfoLog(shader, r, nullptr, s.data());	// copy txt to s
		}
		throw std::logic_error("glCompileShader failed: err msg = " + s);
	}
	return GLShader(shader);
}


inline GLShader LoadGLVertexShader(std::initializer_list<std::string_view>&& codes_) {
	return LoadGLShader(GL_VERTEX_SHADER, std::move(codes_));
}


inline GLShader LoadGLFragmentShader(std::initializer_list<std::string_view>&& codes_) {
	return LoadGLShader(GL_FRAGMENT_SHADER, std::move(codes_));
}


inline GLProgram LinkGLProgram(GLuint const& vs, GLuint const& fs) {
	auto program = glCreateProgram();
	if (!program)
		throw std::logic_error("glCreateProgram failed.");
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	GLint r = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &r);
	if (!r) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &r);
		std::string s;
		if (r) {
			s.resize(r);
			glGetProgramInfoLog(program, r, nullptr, s.data());
		}
		throw std::logic_error("glLinkProgram failed: err msg = " + s);
	}
	return GLProgram(program);
}
