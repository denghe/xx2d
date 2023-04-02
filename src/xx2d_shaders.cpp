﻿#include "xx2d.h"

namespace xx {

	void ShaderManager::GLInit() {
		// set gl global env for all shaders

#ifndef __EMSCRIPTEN__
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(65535);
		glPointSize(5);

		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

		glDisable(GL_CULL_FACE);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);

		CheckGLError();
	}
	void ShaderManager::Init() {
		// make all
		shaders[Shader_Quad::index] = xx::Make<Shader_Quad>();
		shaders[Shader_QuadInstance::index] = xx::Make<Shader_QuadInstance>();
		shaders[Shader_LineStrip::index] = xx::Make<Shader_LineStrip>();
		shaders[Shader_TexVerts::index] = xx::Make<Shader_TexVerts>();
		shaders[Shader_Verts::index] = xx::Make<Shader_Verts>();
		// ...

		// init all
		for (auto& s : shaders) {
			s->Init(this);
		}

		// set default
		cursor = Shader_Quad::index;
	}

	void ShaderManager::ClearCounter() {
		drawCall = drawVerts = drawLinePoints = 0;
	}

	void ShaderManager::Begin() {
		shaders[cursor]->Begin();
	}

	void ShaderManager::End() {
		shaders[cursor]->End();
	}
}
