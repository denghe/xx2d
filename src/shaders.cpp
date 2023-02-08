#include "pch.h"

namespace xx {

	void ShaderManager::Init() {
		// set gl global env for all shaders

		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(65535);
		glPointSize(5);

		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		glDisable(GL_CULL_FACE);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glActiveTexture(GL_TEXTURE0);

		CheckGLError();


		// make all
		shaders[Shader_XyUvC::index] = xx::Make<Shader_XyUvC>();
		shaders[Shader_XyC::index] = xx::Make<Shader_XyC>();
		// ... more make here

		// init all
		for (auto& s : shaders) {
			s->Init(this);
		}

		// set default
		cursor = Shader_XyUvC::index;
	}

	void ShaderManager::Begin() {
		drawCall = drawQuads = drawLinePoints = 0;
		shaders[cursor]->Begin();
	}

	void ShaderManager::End() {
		shaders[cursor]->End();
	}

	size_t ShaderManager::GetDrawCall() {
		return drawCall
			+ (RefShader<Shader_XyC>().pointsCount > 0 ? 1 : 0)
			+ RefShader<Shader_XyUvC>().texsCount
			;
	}

	size_t ShaderManager::GetDrawQuads() {
		auto& s = RefShader<Shader_XyUvC>();
		size_t j = 0;
		for (size_t i = 0; i < s.texsCount; i++) {
			j += s.texs[i].second;
		}
		return drawQuads + j;
	}

	size_t ShaderManager::GetDrawLines() {
		auto& s = RefShader<Shader_XyC>();
		return drawLinePoints + s.indexsCount;
	}

}
