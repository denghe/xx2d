#pragma once
#include "pch.h"

struct Engine {
	/**********************************************************************************/
	// window info
	GLsizei w = 1280, h = 720;

	/**********************************************************************************/
	// shader members
	GLShader v, f;
	GLProgram p;
	GLint uCxy = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
	GLVertexArrays va;
	GLBuffer vb, ib;

	/**********************************************************************************/
	// file system

	std::vector<std::string> searchPaths;
	std::filesystem::path tmpPath;

	// add relative base dir to searchPaths
	void SearchPathAdd(std::string_view const& dir);

	// search paths revert to default
	void SearchPathReset();

	// search file by searchPaths + fn. not found return ""
	std::string GetFullPath(std::string_view const& fn);

	// read all data by GetFullPath( fn )
	xx::Data ReadAllBytes(std::string_view const& fn);

	/**********************************************************************************/
	// texture cache system

	std::unordered_map<std::string, xx::Shared<GLTexture>, xx::StringHasher<>, std::equal_to<void>> textureCache;

	// todo

	/**********************************************************************************/
	// auto batch
	static const size_t maxVertNums = 65536;
	static const size_t maxQuadNums = maxVertNums / 4;
	static const size_t maxIndexNums = maxQuadNums * 6;

	GLuint autoBatchLastTextureId = 0;
	size_t autoBatchTexsCount = 0;
	size_t autoBatchQuadVertsCount = 0;

	std::pair<GLuint, int> autoBatchTexs[maxQuadNums];	// tex id + count
	QuadVerts autoBatchQuadVerts[maxQuadNums];

	void AutoBatchBegin();
	void AutoBatchDrawQuad(GLTexture& tex, QuadVerts const& qvs);
	void AutoBatchCommit();
	
	/**********************************************************************************/
	// for game loop
	void Init();
	void UpdateBegin();
	void UpdateEnd();


	// ...
};
