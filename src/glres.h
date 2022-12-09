#pragma once
#include "pch.h"
//#include <glad/gl.h>
#include <glad/glad.h>

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
