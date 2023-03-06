#include "xx2d_pch.h"

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
		shaders[Shader_Quad::index] = xx::Make<Shader_Quad>();
		shaders[Shader_QuadInstance::index] = xx::Make<Shader_QuadInstance>();
		shaders[Shader_LineStrip::index] = xx::Make<Shader_LineStrip>();
		shaders[Shader_Verts::index] = xx::Make<Shader_Verts>();
		// ...

		// init all
		for (auto& s : shaders) {
			s->Init(this);
		}

		// set default
		cursor = Shader_Quad::index;
	}

	void ShaderManager::Begin() {
		drawCall = drawVerts = drawLinePoints = 0;
		shaders[cursor]->Begin();
	}

	void ShaderManager::End() {
		shaders[cursor]->End();
	}

	size_t ShaderManager::GetDrawCall() {
		return drawCall
			+ (RefShader<Shader_LineStrip>().pointsCount > 0 ? 1 : 0)
			+ RefShader<Shader_Quad>().texsCount
			+ RefShader<Shader_Verts>().texsCount
			+ (RefShader<Shader_QuadInstance>().quadCount > 0 ? 1 : 0)
			// ...
			;
	}

	size_t ShaderManager::GetDrawVerts() {
		size_t j = 0;
		{
			auto& s = RefShader<Shader_Quad>();
			for (size_t i = 0; i < s.texsCount; i++) {
				j += s.texs[i].second * 6;
			}
		}
		{
			auto& s = RefShader<Shader_QuadInstance>();
			j += s.quadCount * 6;
		}
		{
			auto& s = RefShader<Shader_Verts>();
			j += s.indexsCount;
		}
		// ...
		return drawVerts + j;
	}

	size_t ShaderManager::GetDrawLines() {
		auto& s = RefShader<Shader_LineStrip>();
		return drawLinePoints + s.indexsCount;
	}
}
