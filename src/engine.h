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
	// others
	FileSystem fileSystem;
	TextureCache textureCache;

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
	void GLInit();
	void GLUpdateBegin();
	void GLUpdateEnd();


	// ...
};
