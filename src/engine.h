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

	// read all data by full path
	xx::Data ReadAllBytesWithFullPath(std::string_view const& fp, bool autoUnzip = true);

	// read all data by GetFullPath( fn )
	std::pair<xx::Data, std::string> ReadAllBytes(std::string_view const& fn, bool autoUnzip = true);

	/**********************************************************************************/
	// fonts

	BMFont LoadBMFont(std::string_view const& fn);

	// ...


	/**********************************************************************************/
	// texture & cache

	// key: full path
	std::unordered_map<std::string, xx::Shared<GLTexture>, xx::StringHasher<>, std::equal_to<void>> textureCache;

	// load texture from file
	GLTexture LoadTexture(std::string_view const& fn);

	// load + insert or get texture from texture cache
	xx::Shared<GLTexture> LoadTextureFromCache(std::string_view const& fn);

	// unload texture from texture cache by full path
	void UnloadTextureFromCache(std::string_view const& fn);

	// unload texture from texture cache by texture
	void UnloadTextureFromCache(xx::Shared<GLTexture> const& t);

	// delete from textureCache where sharedCount == 1. return affected rows
	size_t RemoveUnusedFromTextureCache();


	/**********************************************************************************/
	// TPData & frame cache( texture does not insert into textureCache )

	// key: frame key in plist( texture packer export .plist )
	std::unordered_map<std::string, xx::Shared<Frame>, xx::StringHasher<>, std::equal_to<void>> frameCache;

	// load texture packer's plist content from file & return
	TPData LoadTPData(std::string_view const& fn);

	// load plist frames into cache. duplicate key name will throw exception. auto get texture from cache if exists.
	void LoadFramesFromCache(TPData const& tpd);

	// unload plist frames from cache
	void UnloadFramesFromCache(TPData const& tpd);

	// delete from cache where sharedCount == 1. return removed frame count
	size_t RemoveUnusedFromFrameCache();


	/**********************************************************************************/
	// window & input

	float w = 1920, h = 1080, hw = w/2, hh = h/2;
	std::array<XY, 10> ninePoints;
	void SetWH(float w, float h);	// will fill w, h, hw, hh, ninePoints

	XY mousePosition{ (float)w/2, (float)h/2 };
	std::deque<uint32_t> kbdInputs;		// for store keyboard typed chars.  pop_front for use
	std::array<uint16_t, 512> kbdStates{};	// for scan keyboard down up
	std::array<uint8_t, 32> mbtnStatus{};	// mouse button states


	/**********************************************************************************/
	// shader

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
	size_t drawCall = 0;	// cleanup when EngineUpdateBegin. +1 when glDrawXxxxx
	size_t GetDrawCall();

	std::unique_ptr<std::pair<GLuint, int>[]> autoBatchTexs = std::make_unique<std::pair<GLuint, int>[]>(maxQuadNums);	// tex id + count
	std::unique_ptr<QuadVerts[]> autoBatchQuadVerts = std::make_unique<QuadVerts[]>(maxQuadNums);

	void AutoBatchBegin();
	void AutoBatchDrawQuad(GLTexture& tex, QuadVerts const& qv);
	void AutoBatchCommit();


	/**********************************************************************************/
	// game loop

	void EngineInit();
	void EngineUpdateBegin();
	void EngineUpdateEnd();
	void EngineDestroy();

	// ...
};
