#pragma once
#include "xx2d.h"

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

namespace xx {

	enum class GLResTypes {
		Shader, Program, VertexArrays, Buffer, Texture, FrameBuffer
	};

	template<GLResTypes T, typename...VS>
	struct GLRes {

		std::tuple<GLuint, VS...> vs;

		GLRes(GLuint i) : vs(std::make_tuple(i)) {}

		template<typename...Args>
		GLRes(GLuint i, Args&&... args) : vs(std::make_tuple(i, std::forward<Args>(args)...)) {}

		operator GLuint const& () const { return std::get<0>(vs); }
		GLuint* Get() { return &std::get<0>(vs); }

		GLRes(GLRes const&) = delete;
		GLRes& operator=(GLRes const&) = delete;
		GLRes() = default;
		GLRes(GLRes&& o) noexcept {
			std::swap(vs, o.vs);
		}
		GLRes& operator=(GLRes&& o) noexcept {
			std::swap(vs, o.vs);
			return *this;
		}

		~GLRes() {
			if (!std::get<0>(vs)) return;
			if constexpr (T == GLResTypes::Shader) {
				glDeleteShader(std::get<0>(vs));
			}
			if constexpr (T == GLResTypes::Program) {
				glDeleteProgram(std::get<0>(vs));
			}
			if constexpr (T == GLResTypes::VertexArrays) { 
				glDeleteVertexArrays(1, &std::get<0>(vs));
			}
			if constexpr (T == GLResTypes::Buffer) { 
				glDeleteBuffers(1, &std::get<0>(vs)); 
			}
			if constexpr (T == GLResTypes::Texture) {
				glDeleteTextures(1, &std::get<0>(vs));
//				std::cout << "glDeleteTextures " << (GLuint)std::get<0>(vs) << std::endl;
			}
			if constexpr (T == GLResTypes::FrameBuffer) {
				glDeleteFramebuffers(1, &std::get<0>(vs));
			}
			std::get<0>(vs) = 0;
		}
	};

	using GLShader = GLRes<GLResTypes::Shader>;

	using GLProgram = GLRes<GLResTypes::Program>;

	using GLVertexArrays = GLRes<GLResTypes::VertexArrays>;

	using GLBuffer = GLRes<GLResTypes::Buffer>;

	using GLFrameBuffer = GLRes<GLResTypes::FrameBuffer>;

	using GLTextureCore = GLRes<GLResTypes::Texture>;


	void GLTexParmCore(GLuint const& a1 = GL_NEAREST/*GL_LINEAR*/, GLuint const& a2 = GL_REPEAT/*GL_CLAMP_TO_EDGE*/);

	void GLTexParm(GLuint const& t, GLuint const& a1 = GL_NEAREST/*GL_LINEAR*/, GLuint const& a2 = GL_REPEAT/*GL_CLAMP_TO_EDGE*/);

	GLuint LoadGLTexture_core(int textureUnit = 0);


	struct GLTexture : GLRes<GLResTypes::Texture, GLsizei, GLsizei, std::string> {
		using BT = GLRes<GLResTypes::Texture, GLsizei, GLsizei, std::string>;
		using BT::BT;

		void SetGLTexParm(GLuint const& a1 = GL_NEAREST/*GL_LINEAR*/, GLuint const& a2 = GL_REPEAT/*GL_CLAMP_TO_EDGE*/) {
			GLTexParm(std::get<0>(vs), a1, a2);
		}

		auto const& Width() const { return std::get<1>(vs); }
		auto& Width() { return std::get<1>(vs); }
		auto const& Height() const { return std::get<2>(vs); }
		auto& Height() { return std::get<2>(vs); }
		auto& FileName() { return std::get<3>(vs); }
		auto const& FileName() const { return std::get<3>(vs); }

		float FloatWidth() const { return std::get<1>(vs); }
		float FloatHeight() const { return std::get<2>(vs); }
	};

	// fn must be absolute path. GetFullPath recommend
	GLTexture LoadGLTexture(std::string_view const& buf, std::string_view const& fullPath);



	GLShader LoadGLShader(GLenum const& type, std::initializer_list<std::string_view>&& codes_);

	GLShader LoadGLVertexShader(std::initializer_list<std::string_view>&& codes_);

	GLShader LoadGLFragmentShader(std::initializer_list<std::string_view>&& codes_);

	GLProgram LinkGLProgram(GLuint const& vs, GLuint const& fs);

	GLFrameBuffer MakeGLFrameBuffer();
	GLTexture MakeGLFrameBufferTexture(uint32_t const& w, uint32_t const& h, bool const& hasAlpha);
	void BindGLFrameBufferTexture(GLuint const& f, GLuint const& t);
	void UnbindGLFrameBuffer();

	//std::pair<GLFrameBuffer, GLTexture> MakeGLFrameBuffer(uint32_t const& w, uint32_t const& h);
}
