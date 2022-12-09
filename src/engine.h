#pragma once
#include "pch.h"

struct Engine {
	/**********************************************************************************/
	// file system

	std::vector<std::string> searchPaths;
	std::filesystem::path tmpPath;

	// add relative base dir to searchPaths
	void SearchPathAdd(std::string_view dir);

	// search paths revert to default
	void SearchPathReset();

	// search file by searchPaths + fn. not found return ""
	std::string GetFullPath(std::string_view fn);

	// read all data by GetFullPath( fn )
	xx::Data ReadAllBytes(std::string_view fn);



	/**********************************************************************************/
	// texture cache system

	// key: full path
	std::unordered_map<std::string, xx::Shared<GLTexture>, xx::StringHasher<>, std::equal_to<void>> textureCache;

	// load + insert or get texture from texture cache
	xx::Shared<GLTexture> TextureCacheLoad(std::string_view fn);

	// unload texture from texture cache by full path
	void TextureCacheUnload(std::string_view fn);

	// unload texture from texture cache by texture
	void TextureCacheUnload(xx::Shared<GLTexture> const& t);



	/**********************************************************************************/
	// window info
	GLsizei w = 1920, h = 1080;



	/**********************************************************************************/
	// shader members
	GLShader v, f;
	GLProgram p;
	GLint uCxy = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
	GLVertexArrays va;
	GLBuffer vb, ib;



	/**********************************************************************************/
	// auto batch
	static const size_t maxVertNums = 65536;
	static const size_t maxQuadNums = maxVertNums / 4;
	static const size_t maxIndexNums = maxQuadNums * 6;

	GLuint autoBatchLastTextureId = 0;
	size_t autoBatchTexsCount = 0;
	size_t autoBatchQuadVertsCount = 0;

	std::unique_ptr<std::pair<GLuint, int>[]> autoBatchTexs = std::make_unique<std::pair<GLuint, int>[]>(maxQuadNums);	// tex id + count
	std::unique_ptr<QuadVerts[]> autoBatchQuadVerts = std::make_unique<QuadVerts[]>(maxQuadNums);

	void AutoBatchBegin();
	void AutoBatchDrawQuad(GLTexture& tex, QuadVerts const& qvs);
	void AutoBatchCommit();
	


	/**********************************************************************************/
	// game loop
	void EngineInit();
	void EngineUpdateBegin();
	void EngineUpdateEnd();
	void EngineDestroy();

	// ...
};
