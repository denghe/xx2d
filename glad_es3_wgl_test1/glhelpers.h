#pragma once
#include "pch.h"

union Color4b {
	struct {
		uint8_t r, g, b, a;
	};
	uint32_t data = 0;
};

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
	GLRes(GLRes&& o) {
		std::swap(vs, o.vs);
	}
	GLRes& operator=(GLRes&& o) {
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
using Shader = GLRes<GLResTypes::Shader>;
using Program = GLRes<GLResTypes::Program>;
using VertexArrays = GLRes<GLResTypes::Vertexss>;
using Buffer = GLRes<GLResTypes::Buffer>;
using Texture = GLRes<GLResTypes::Texture, GLsizei, GLsizei>;

xx::Shared<Texture> MakeTexture(std::string_view const& fn);
Shader LoadShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_);
Shader LoadVertexShader(std::initializer_list<std::string_view>&& codes_);
Shader LoadFragmentShader(std::initializer_list<std::string_view>&& codes_);
Program LinkProgram(GLuint const& vs, GLuint const& fs);
