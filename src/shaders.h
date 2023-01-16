#pragma once
#include "pch.h"

struct Shaders {
	static std::string_view vsSrc;
	static std::string_view fsSrc;
};

/***************************************************************************************************/

struct ShaderManager;
struct Shader {
	static const size_t index = -1;	// index at sm->shaders
	ShaderManager* sm{};

	GLShader v, f;
	GLProgram p;

	static const size_t maxVertNums = 65535;	// 65535 for primitive restart index

	virtual void Init(ShaderManager*) = 0;
	virtual void Begin() = 0;
	virtual void Commit() = 0;
	virtual ~Shader() {}
};

/***************************************************************************************************/

struct ShaderManager {
	// all shader instance container
	std::array<xx::Shared<Shader>, 2> shaders{};

	// store current shaders index
	size_t cursor = -1;

	// performance counters
	size_t drawCall{}, drawQuads{}, drawLines{};	// set zero by begin

	// make & call all shaders Init
	void Init();

	// zero counters & begin default shader
	void Begin();

	// commit current shader
	void Commit();

	template<typename T, typename ENABLED = std::enable_if_t<std::is_base_of_v<Shader, T>>>
	T& Get() {
		if (cursor != T::index) {
			shaders[cursor]->Commit();
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
	void Commit() override;

	QuadVerts& DrawQuadBegin(GLTexture& tex);	// need fill & commit
	void DrawQuadCommit();

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
	void Commit() override;

	XYRGBA8* DrawLineBegin(int numPoints);	// need fill & commit
	void DrawLineCommit();
};


// ... more shader struct here
