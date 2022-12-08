#include "pch.h"
#define GLAD_MALLOC(sz)       malloc(sz)
#define GLAD_FREE(ptr)        free(ptr)
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>


Texture LoadTexture(std::string_view const& fn) {
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
		return { t, width, height };
	}
	throw std::logic_error(xx::ToString("unsupported texture type. fn = ", fn));
}

Shader LoadShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_) {
	assert(codes_.size() && (type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER));
	auto&& shader = glCreateShader(type);	// 申请 shader 上下文. 返回 0 表示失败, 参数：GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER
	if (!shader) throw std::logic_error(xx::ToString("glCreateShader(", type, ") failed."));
	std::vector<GLchar const*> codes;
	codes.resize(codes_.size());
	std::vector<GLint> codeLens;
	codeLens.resize(codes_.size());
	auto ss = codes_.begin();
	for (size_t i = 0; i < codes.size(); ++i) {
		codes[i] = (GLchar const*)ss[i].data();
		codeLens[i] = (GLint)ss[i].size();
	}
	glShaderSource(shader, (GLsizei)codes_.size(), codes.data(), codeLens.data());	// 参数：目标 shader, 代码段数, 段数组, 段长度数组
	glCompileShader(shader);	// 编译
	GLint r = GL_FALSE;	// 状态容器
	glGetShaderiv(shader, GL_COMPILE_STATUS, &r);	// 查询是否编译成功
	if (!r) {	// 失败
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &r);	// 查询日志信息文本长度
		std::string s;
		if (r) {
			s.resize(r);
			glGetShaderInfoLog(shader, r, nullptr, s.data());	// 复制错误文本
		}
		throw std::logic_error("glCompileShader failed: err msg = " + s);
	}
	return Shader(shader);
}
Shader LoadVertexShader(std::initializer_list<std::string_view>&& codes_) {
	return LoadShader(GL_VERTEX_SHADER, std::move(codes_));
}
Shader LoadFragmentShader(std::initializer_list<std::string_view>&& codes_) {
	return LoadShader(GL_FRAGMENT_SHADER, std::move(codes_));
}

Program LinkProgram(GLuint const& vs, GLuint const& fs) {
	// 创建一个 program. 返回 0 表示失败
	auto program = glCreateProgram();
	if (!program) throw std::logic_error("glCreateProgram failed.");
	glAttachShader(program, vs);	// 向 program 附加 vs
	glAttachShader(program, fs);	// 向 program 附加 ps
	glLinkProgram(program);	// 链接
	GLint r = GL_FALSE;	// 状态容器
	glGetProgramiv(program, GL_LINK_STATUS, &r);	// 查询链接是否成功
	if (!r) {	// 失败
		// 查询日志信息文本长度
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &r);
		std::string s;
		if (r) {
			s.resize(r);
			glGetProgramInfoLog(program, r, nullptr, s.data());	// 复制错误文本
		}
		throw std::logic_error("glLinkProgram failed: err msg = " + s);
	}
	return Program(program);
}
