#include "xx2d.h"

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

	void ShaderManager::Begin() {
		drawCall = drawVerts = drawLinePoints = 0;
		shaders[cursor]->Begin();
	}

	void ShaderManager::End() {
		shaders[cursor]->End();
	}
}
