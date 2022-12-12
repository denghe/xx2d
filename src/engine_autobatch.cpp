#include "pch.h"
#include "engine.h"

void Engine::AutoBatchDrawQuad(GLTexture& tex, QuadVerts const& qvs) {
	if (autoBatchQuadVertsCount == maxQuadNums) {
		AutoBatchCommit();
	}
	if (autoBatchLastTextureId != tex) {
		autoBatchLastTextureId = tex;
		autoBatchTexs[autoBatchTexsCount].first = tex;
		autoBatchTexs[autoBatchTexsCount].second = 1;
		++autoBatchTexsCount;
	} else {
		autoBatchTexs[autoBatchTexsCount - 1].second += 1;
	}
	memcpy(&autoBatchQuadVerts[autoBatchQuadVertsCount], qvs.data(), sizeof(qvs));
	++autoBatchQuadVertsCount;
};

void Engine::AutoBatchCommit() {
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVerts) * autoBatchQuadVertsCount, autoBatchQuadVerts.get(), GL_DYNAMIC_DRAW);

	for (size_t i = 0, j = 0; i < autoBatchTexsCount; i++) {
		glBindTexture(GL_TEXTURE_2D, autoBatchTexs[i].first);
		auto n = (GLsizei)(autoBatchTexs[i].second * 6);
		glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_SHORT, (GLvoid*)j);
		j += n * 2;
	}
	CheckGLError();

	autoBatchLastTextureId = 0;
	autoBatchTexsCount = 0;
	autoBatchQuadVertsCount = 0;
}

void Engine::AutoBatchDrawSprite(Sprite const& s) {
	AutoBatchDrawQuad(*s.tex, s.verts);
}

void Engine::AutoBatchDrawSprite(xx::Shared<Sprite> const& s) {
	AutoBatchDrawQuad(*s->tex, s->verts);
}
