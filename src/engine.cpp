#include "pch.h"

void Engine::EngineInit() {
	lastTime = xx::NowSteadyEpochSeconds();
	lastDelta = 0;
	SearchPathReset();

	CheckGLError();

	v = LoadGLVertexShader({ Shaders::vsSrc });
	f = LoadGLFragmentShader({ Shaders::fsSrc });
	p = LinkGLProgram(v, f);

	uCxy = glGetUniformLocation(p, "uCxy");
	uTex0 = glGetUniformLocation(p, "uTex0");

	aPos = glGetAttribLocation(p, "aPos");
	aTexCoord = glGetAttribLocation(p, "aTexCoord");
	aColor = glGetAttribLocation(p, "aColor");
	CheckGLError();



	glGenVertexArrays(1, &va.Ref());
	glBindVertexArray(va);
	assert((GLuint*)&vb + 1 == (GLuint*)&ib);
	glGenBuffers(2, (GLuint*)&vb);

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, sizeof(XYUVRGBA8), 0);
	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aTexCoord, 2, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, u));
	glEnableVertexAttribArray(aTexCoord);
	glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(XYUVRGBA8), (GLvoid*)offsetof(XYUVRGBA8, r));
	glEnableVertexAttribArray(aColor);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	{
		auto idxs = std::make_unique<GLushort[]>(maxIndexNums);
		for (size_t i = 0; i < maxVertNums / 4; i++) {
			auto p = idxs.get() + i * 6;
			auto v = i * 4;
			p[0] = uint16_t(v + 0);
			p[1] = uint16_t(v + 1);
			p[2] = uint16_t(v + 2);
			p[3] = uint16_t(v + 0);
			p[4] = uint16_t(v + 2);
			p[5] = uint16_t(v + 3);
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * maxIndexNums, idxs.get(), GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CheckGLError();



	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	glDisable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glActiveTexture(GL_TEXTURE0);

	CheckGLError();
}

void Engine::EngineUpdateBegin() {
	lastDelta = xx::NowSteadyEpochSeconds(lastTime);
	drawCall = 0;

	assert(w >= 0 && h >= 0);
	glViewport(0, 0, w, h);
	glDepthMask(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(false);


	glUseProgram(p);
	glUniform1i(uTex0, 0);
	glUniform2f(uCxy, w / 2, h / 2);

	glBindVertexArray(va);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);	// 已知问题：这句不加要出错
}

void Engine::EngineUpdateEnd() {
	if (autoBatchQuadVertsCount) {
		AutoBatchCommit();
	}
	else {
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);
	}
}

void Engine::EngineDestroy() {
	textureCache.clear();
	// ...
}

void Engine::SetWH(float w, float h) {
	this->w = w;
	this->h = h;
	hw = (float)w / 2;
	hh = (float)h / 2;
	ninePoints[1] = { -hw, -hh };
	ninePoints[2] = { 0, -hh };
	ninePoints[3] = { hw, -hh };
	ninePoints[4] = { -hw, 0 };
	ninePoints[5] = { 0, 0 };
	ninePoints[6] = { hw, 0 };
	ninePoints[7] = { -hw, hh };
	ninePoints[8] = { 0, hh };
	ninePoints[9] = { hw, hh };
}
