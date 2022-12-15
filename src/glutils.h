#pragma once
#include "pch.h"
#include "glres.h"

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

// fn must be absolute path. GetFullPath recommend
GLTexture LoadGLTexture(std::string_view const& buf, std::string_view const& fullPath);

GLShader LoadGLShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_);

GLShader LoadGLVertexShader(std::initializer_list<std::string_view>&& codes_);

GLShader LoadGLFragmentShader(std::initializer_list<std::string_view>&& codes_);

GLProgram LinkGLProgram(GLuint const& vs, GLuint const& fs);
