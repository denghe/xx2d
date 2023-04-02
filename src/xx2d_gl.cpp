﻿#include "xx2d.h"

#ifndef __EMSCRIPTEN__
#define GLAD_MALLOC(sz)       malloc(sz)
#define GLAD_FREE(ptr)        free(ptr)
#define GLAD_GL_IMPLEMENTATION
#include <glad.h>
#endif

#define STBI_NO_JPEG
//#define STBI_NO_PNG
#define STBI_NO_GIF
#define STBI_NO_PSD
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_HDR
#define STBI_NO_TGA
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace xx {

	GLuint LoadGLTexture_core() {
		GLuint t{};
		glGenTextures(1, &t);
		glActiveTexture(GL_TEXTURE0/* + textureUnit*/);
		glBindTexture(GL_TEXTURE_2D, t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST/*GL_LINEAR*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST/*GL_LINEAR*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT/*GL_CLAMP_TO_EDGE*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT/*GL_CLAMP_TO_EDGE*/);
		return t;
	}

	GLTexture LoadGLTexture(std::string_view const& buf, std::string_view const& fullPath) {
		if (buf.size() <= 12) {
			throw std::logic_error(xx::ToString("texture file size too small. fn = ", fullPath));
		}

		/***********************************************************************************************************************************/
		// etc 2.0 / pkm2

		if (buf.starts_with("PKM 20"sv) && buf.size() >= 16) {
			auto p = (uint8_t*)buf.data();
			uint16_t format = (p[6] << 8) | p[7];				// 1 ETC2_RGB_NO_MIPMAPS, 3 ETC2_RGBA_NO_MIPMAPS
			uint16_t encodedWidth = (p[8] << 8) | p[9];			// 4 align width
			uint16_t encodedHeight = (p[10] << 8) | p[11];		// 4 align height
			uint16_t width = (p[12] << 8) | p[13];				// width
			uint16_t height = (p[14] << 8) | p[15];				// height
			assert(width > 0 && height > 0 && encodedWidth >= width && encodedWidth - width < 4 && encodedHeight >= height && encodedHeight - height < 4);
			if (format == 1) {
				assert(buf.size() == 16 + encodedWidth * encodedHeight / 2);
			} else if (format == 3) {
				assert(buf.size() == 16 + encodedWidth * encodedHeight);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 8 - 4 * (width & 0x1));
			} else {
				throw std::logic_error(xx::ToString("unsppported PKM 20 format. only support ETC2_RGB_NO_MIPMAPS & ETC2_RGBA_NO_MIPMAPS. fn = ", fullPath));
			}
			auto t = LoadGLTexture_core();
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, format == 3 ? GL_COMPRESSED_RGBA8_ETC2_EAC : GL_COMPRESSED_RGB8_ETC2, (GLsizei)width, (GLsizei)height, 0, (GLsizei)(buf.size() - 16), p + 16);
			glBindTexture(GL_TEXTURE_2D, 0);
			CheckGLError();
			return { t, width, height, fullPath };
		}

		/***********************************************************************************************************************************/
		// png

		else if (buf.starts_with("\x89\x50\x4e\x47\x0d\x0a\x1a\x0a"sv)) {
			int w, h, comp;
			if (auto image = stbi_load(std::string(fullPath).c_str(), &w, &h, &comp, STBI_rgb_alpha)) {
				auto c = comp == 3 ? GL_RGB : GL_RGBA;
				if (comp == 4) {
					glPixelStorei(GL_UNPACK_ALIGNMENT, 8 - 4 * (w & 0x1));
				}
				auto t = LoadGLTexture_core();
				glTexImage2D(GL_TEXTURE_2D, 0, c, w, h, 0, c, GL_UNSIGNED_BYTE, image);
				glBindTexture(GL_TEXTURE_2D, 0);
				stbi_image_free(image);
				return { t, w, h, fullPath };
			} else {
				std::logic_error(xx::ToString("failed to load texture. fn = ", fullPath));
			}
		}

		/***********************************************************************************************************************************/
		// todo: more format support here

		throw std::logic_error(xx::ToString("unsupported texture type. fn = ", fullPath));
	}
	

	GLShader LoadGLShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_) {
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


	GLShader LoadGLVertexShader(std::initializer_list<std::string_view>&& codes_) {
		return LoadGLShader(GL_VERTEX_SHADER, std::move(codes_));
	}


	GLShader LoadGLFragmentShader(std::initializer_list<std::string_view>&& codes_) {
		return LoadGLShader(GL_FRAGMENT_SHADER, std::move(codes_));
	}


	GLProgram LinkGLProgram(GLuint const& vs, GLuint const& fs) {
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

	GLFrameBuffer MakeGLFrameBuffer() {
		GLuint f{};
		glGenFramebuffers(1, &f);
		glBindFramebuffer(GL_FRAMEBUFFER, f);
		CheckGLError();
		return GLFrameBuffer(f);
	}

	GLTexture MakeGLFrameBufferTexture(uint32_t const& w, uint32_t const& h, bool const& hasAlpha) {
		auto t = LoadGLTexture_core();
		auto c = hasAlpha ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, c, w, h, 0, c, GL_UNSIGNED_BYTE, {});
		glBindTexture(GL_TEXTURE_2D, 0);
		return GLTexture(t, w, h, "");
	}

	void BindGLFrameBufferTexture(GLuint const& f, GLuint const& t) {
		glBindFramebuffer(GL_FRAMEBUFFER, f);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t, 0);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	void UnbindGLFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//std::pair<GLFrameBuffer, GLTexture> MakeGLFrameBuffer(uint32_t const& w, uint32_t const& h) {
	//	auto f = MakeGLFrameBuffer();

	//	auto t = LoadGLTexture_core();
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, {});
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t, 0);

	//	//GLuint r{};
	//	//glGenRenderbuffers(1, &r);
	//	//glBindRenderbuffer(GL_RENDERBUFFER, r);
	//	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	//	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, r);

	//	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	return { std::move(f), GLTexture(t, w, h, "") };
	//}
}
