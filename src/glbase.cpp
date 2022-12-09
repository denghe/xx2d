#include "pch.h"
#include "glbase.h"

// todo: 贴图数组支持。绘制前需要先用要用到的贴图，填进数组。Texture 对象将附带存储 其对应的 下标。用的时候 顶点数据 安排一个 贴图下标 的存储位置? 还是说必须配合 draw instance 方案, 再用一个 buffer 存每个实例用哪个 tex 下标？

void GLBase::GLInit() {
	CheckGLError();

	// 初始化 shader 及其参数位置变量

	v = LoadVertexShader({ Shaders::vsSrc });
	f = LoadFragmentShader({ Shaders::fsSrc });
	p = LinkProgram(v, f);

	uCxy = glGetUniformLocation(p, "uCxy");
	uTex0 = glGetUniformLocation(p, "uTex0");

	aPos = glGetAttribLocation(p, "aPos");
	aTexCoord = glGetAttribLocation(p, "aTexCoord");
	aColor = glGetAttribLocation(p, "aColor");
	CheckGLError();

	// 初始化 shader 要用到的 buffer

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

	// 这样先声明 再提交 sub 3070 实测更慢
	//glBufferData(GL_ARRAY_BUFFER, sizeof(XYUVIJRGBA8) * maxVertNums, nullptr, GL_DYNAMIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(XYUVIJRGBA8) * 4 * autoBatchNumQuads, verts);

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

	// 初始化 gl 运行环境 特性支持

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

void GLBase::GLUpdateBegin() {
	
	// cleanup

	assert(w >= 0 && h >= 0);
	glViewport(0, 0, w, h);
	glDepthMask(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(false);

	// 启用 shader & 传参

	glUseProgram(p);
	glUniform1i(uTex0, 0);
	glUniform2f(uCxy, w / 2, h / 2);

	glBindVertexArray(va);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);	// 已知问题：这句不加要出错
}

void GLBase::AutoBatchDrawQuad(Texture& tex, QuadVerts const& qvs) {
	if (autoBatchLastTextureId != tex) {
		autoBatchLastTextureId = tex;
		autoBatchTexs[autoBatchTexsCount].first = tex;
		autoBatchTexs[autoBatchTexsCount].second = 1;
		++autoBatchTexsCount;
	} else {
		autoBatchTexs[autoBatchTexsCount - 1].second += 1;
	}

	memcpy(autoBatchQuadVerts + autoBatchQuadVertsCount, qvs.data(), sizeof(qvs));
	++autoBatchQuadVertsCount;

	if (autoBatchQuadVertsCount == maxQuadNums) {
		AutoBatchCommit();
	}
};

void GLBase::AutoBatchCommit() {

	// 提交 顶点数据到 buf

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVerts) * autoBatchQuadVertsCount, autoBatchQuadVerts, GL_DYNAMIC_DRAW);

	for (size_t i = 0, j = 0; i < autoBatchTexsCount; i++) {
		glBindTexture(GL_TEXTURE_2D, autoBatchTexs[i].first);
		auto n = (GLsizei)(autoBatchTexs[i].second * 6);
		glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_SHORT, (GLvoid*)j);
		j += n;
	}
	CheckGLError();

	autoBatchLastTextureId = 0;
	autoBatchTexsCount = 0;
	autoBatchQuadVertsCount = 0;
}

void GLBase::GLUpdateEnd() {
	if (autoBatchQuadVertsCount) {
		AutoBatchCommit();
	} else {
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);
	}
}
