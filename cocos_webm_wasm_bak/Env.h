#pragma once
#include <cstdint>
// call by host
void EnvInit(void* scene);

extern "C" {

	int XxmvLoad(char* fileName);

	int SpriteNew(char* spriteFrameName, int parentIndex = -1);
	void SpriteDelete(int index);
	void SpriteSetPosition(int index, float x, float y);
	void SpriteSetZOrder(int index, float z);
	void SpriteSetAnchor(int index, float x, float y);
	void SpriteSetRotation(int index, float r);
	void SpriteSetColor(int index, uint8_t r, uint8_t g, uint8_t b);
	void SpriteSetOpacity(int index, uint8_t o);
	void SpriteSetVisible(int index, bool b);
	void SpriteSetFrameName(int index, char* spriteFrameName);
}
