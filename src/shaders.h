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
		// all shader instance container
		std::array<xx::Shared<Shader>, 3> shaders{};

		// store current shaders index
		size_t cursor = -1;

		// make & call all shaders Init
		void Init();

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
			return *shaders[T::index].template ReinterpretCast<T>();
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

	// 1 vert data
	struct XYUVRGBA8 : XY, UV, RGBA8 {};

	/*
	 1┌────┐2
	  │    │
	 0└────┘3
	*/
	using QuadVerts = std::array<XYUVRGBA8, 4>;


	// for draw quad	// todo: change to vertex mode
	struct Shader_Quad : Shader {
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

	struct QuadInstanceData {
		XY pos{}, anchor{ 0.5, 0.5 }, scale{ 1, 1 };
		float radians{};
		RGBA8 color{ 255, 255, 255, 255 };
		uint16_t texRectX{}, texRectY{}, texRectW{}, texRectH{};
	};

	// for draw quad by drawInstance
	struct Shader_QuadInstance : Shader {
		static const size_t index = 1;	// index at sm->shaders

		GLint uCxy = -1, uTex0 = -1, aVert = -1, aPosAnchor = -1, aScaleRadians = -1, aColor = -1, aTexRect = -1;
		GLVertexArrays va;
		GLBuffer vb, ib;

		static const size_t maxQuadNums = 200000;
		GLuint lastTextureId = 0;
		std::unique_ptr<std::pair<GLuint, int>[]> texs = std::make_unique<std::pair<GLuint, int>[]>(maxQuadNums);	// tex id + count
		size_t texsCount = 0;
		std::unique_ptr<QuadInstanceData[]> quadInstanceDatas = std::make_unique<QuadInstanceData[]>(maxQuadNums);
		size_t quadCount = 0;

		void Init(ShaderManager*) override;
		void Begin() override;
		void End() override;

		void Commit();
		QuadInstanceData& DrawQuadBegin(GLTexture& tex);	// need fill & commit
		void DrawQuadEnd();

		void DrawQuad(GLTexture& tex, QuadInstanceData const* const& qv);	// memcpy & commit
	};

	/***************************************************************************************************/

	// 1 point data ( for draw line strip )
	struct XYRGBA8 : XY, RGBA8 {};

	// for draw line strip
	struct Shader_LineStrip : Shader {
		static const size_t index = 2;	// index at sm->shaders

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
