#pragma once
#include "pch.h"

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
	size_t drawCall{}, drawQuads{}, drawLines{};	// set zero by begin
	size_t GetDrawCall();
	size_t GetDrawQuads();

	template<typename T, typename ENABLED = std::enable_if_t<std::is_base_of_v<Shader, T>>>
	T& GetShader() {
		if (cursor != T::index) {
			shaders[cursor]->End();
			cursor = T::index;
			shaders[T::index]->Begin();
		}
		return *shaders[T::index].ReinterpretCast<T>();
	}
};

/***************************************************************************************************/

// for draw quad
struct Shader_XyUvC : Shader {
	static const size_t index = 0;	// index at sm->shaders

	static const size_t maxQuadNums = maxVertNums / 4;
	static const size_t maxIndexNums = maxQuadNums * 6;

	GLint uCxy = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
	GLVertexArrays va;
	GLBuffer vb, ib;

	GLuint lastTextureId = 0;
	size_t texsCount = 0;
	size_t quadVertsCount = 0;
	std::unique_ptr<std::pair<GLuint, int>[]> texs = std::make_unique<std::pair<GLuint, int>[]>(maxQuadNums);	// tex id + count
	std::unique_ptr<QuadVerts[]> quadVerts = std::make_unique<QuadVerts[]>(maxQuadNums);

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

	static const size_t maxIndexNums = maxVertNums * 1.5;

	GLint uCxy = -1, aPos = -1, aColor = -1;
	GLVertexArrays va;
	GLBuffer vb, ib;

	void Init(ShaderManager*) override;
	void Begin() override;
	void End() override;

	XYRGBA8* DrawLineBegin(int numPoints);	// need fill & commit
	void DrawLineEnd();
};


// ... more shader struct here
