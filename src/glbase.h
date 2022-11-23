#pragma once
#include "pch.h"
#include "glhelpers.h"
#include "sprite.h"

struct GLBase {
	// 外部赋值
	GLsizei w = 1280, h = 720;

	// 内部变量
	Shader v, f;
	Program p;
	GLint uCxy = -1, uTex0 = -1, aPos = -1, aColor = -1, aTexCoord = -1;
	Buffer vb, ib;
	inline static const size_t batchSize = std::numeric_limits<uint16_t>::max() / 6;
	
	// 初始化 opengl 环境
	void GLInit();

	// logic update 之前调用
	void GLUpdate();

	// 内部函数. 执行并返回 显存映射 buf 等待 copy
	decltype(Sprite::verts)* BeginDraw(size_t siz);
	// 内部函数. 在 copy 完 buf 之后调用，完成绘制
	void EndDraw(Texture const& t, size_t siz);

	// 绘制一批 sprite. 要求使用相同 shader, texture, siz 不得超过 batchSize
	void DrawSpriteBatch(Sprite const* ptr, size_t siz);
	void DrawSpriteBatch(Sprite const** ptr, size_t siz);
	void DrawSpriteBatch(xx::Shared<Sprite> const* ptr, size_t siz);

	// 绘制一批 sprite. 无特殊要求。自动分组调用 DrawSpriteBatch
	void DrawSprites(Sprite const* ptr, size_t siz);
	void DrawSprites(Sprite const** ptr, size_t siz);
	void DrawSprites(xx::Shared<Sprite> const* ptr, size_t siz);
};
