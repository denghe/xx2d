#pragma once
#include "pch.h"

namespace xx {

	struct ShaderManager;
	struct Shader {
		static const size_t index = -1;	// index at sm->shaders
		ShaderManager* sm{};

		GLShader v, f;
		GLProgram p;

		static const size_t maxVertNums = 65535;	// 65535 for primitive restart index

		virtual void Init(ShaderManager*) = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual ~Shader() {}
	};

	/***************************************************************************************************/

	struct Engine;
	struct ShaderManager {
		// owner engine
		Engine* eg{};

		// all shader instance container
		std::array<xx::Shared<Shader>, 2> shaders{};

		// store current shaders index
		size_t cursor = -1;

		// make & call all shaders Init
		void Init(Engine*);

		// zero counters & begin default shader
		void Begin();

		// end current shader
		void End();

		// performance counters
		size_t drawCall{}, drawQuads{}, drawLinePoints{};	// set zero by begin
		size_t GetDrawCall();
		size_t GetDrawQuads();
		size_t GetDrawLines();

		// direct ref to shader instance
		template<typename T, typename ENABLED = std::enable_if_t<std::is_base_of_v<Shader, T>>>
		T& RefShader() {
			return *shaders[T::index].ReinterpretCast<T>();
		}

		// for logic: switch + ref to shader instance
		template<typename T>
		T& GetShader() {
			auto&& r = RefShader<T>();
			r.Begin();
			return r;
		}
	};

	/***************************************************************************************************/

	// for draw quad
	struct Shader_XyUvC : Shader {
		static const size_t index = 0;	// index at sm->shaders

		GLint uCxy = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
		GLVertexArrays va;
		GLBuffer vb, ib;

		static const size_t maxQuadNums = maxVertNums / 4;
		static const size_t maxIndexNums = maxQuadNums * 6;
		GLuint lastTextureId = 0;
		std::unique_ptr<std::pair<GLuint, int>[]> texs = std::make_unique<std::pair<GLuint, int>[]>(maxQuadNums);	// tex id + count
		size_t texsCount = 0;
		std::unique_ptr<QuadVerts[]> quadVerts = std::make_unique<QuadVerts[]>(maxQuadNums);
		size_t quadVertsCount = 0;

		void Init(ShaderManager*) override;
		void Begin() override;
		void End() override;

		void Commit();
		QuadVerts& DrawQuadBegin(GLTexture& tex);	// need fill & commit
		void DrawQuadEnd();

		void DrawQuad(GLTexture& tex, QuadVerts const& qv);	// memcpy & commit
	};

	/***************************************************************************************************/

	// for draw line strip
	struct Shader_XyC : Shader {
		static const size_t index = 1;	// index at sm->shaders

		GLint uCxy = -1, aPos = -1, aColor = -1;
		GLVertexArrays va;
		GLBuffer vb, ib;

		static const size_t maxIndexNums = maxVertNums * 1.5;
		std::unique_ptr<XYRGBA8[]> points = std::make_unique<XYRGBA8[]>(maxVertNums);
		size_t pointsCount = 0;
		std::unique_ptr<uint16_t[]> indexs = std::make_unique<uint16_t[]>(maxIndexNums);
		size_t indexsCount = 0;

		void Init(ShaderManager*) override;
		void Begin() override;
		void End() override;

		void Commit();
		XYRGBA8* DrawLineStrip(size_t const& pointsCount);	// fill
		void DrawLineStrip(XYRGBA8* pointsBuf, size_t const& pointsCount);	// memcpy
	};


	// ... more shader struct here

}
