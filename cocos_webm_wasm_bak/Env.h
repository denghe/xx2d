#pragma once
#include <cstdint>
// call by host
void EnvInit(void* scene);

extern "C" {
	/*********************************************************************/

	// load .xxmv file by fileName, return key index
	int XxmvNew(char const* fileName, int fileNameSize);

	// unload .xxmv sprite frames by key index
	void XxmvDelete(int selfKI);

	// return sprite frames .size()
	int XxmvGetFramesCount(int selfKI);

	/*********************************************************************/

	// scene->addChild( create sprite ), return key index
	int SpriteNew(int xxmvKI, int parentIndex);

	// remove sprite from scene childs by key index
	void SpriteDelete(int selfKI);

	// set sprite's sprite frame by xxmv's sprite frames[ index ]
	void SpriteSetXxmvFrame(int selfKI, int xxmvKI, int index);

	void SpriteSetPosition(int selfKI, float x, float y);
	void SpriteSetZOrder(int selfKI, float z);
	void SpriteSetAnchor(int selfKI, float x, float y);
	void SpriteSetRotation(int selfKI, float r);
	void SpriteSetScale(int selfKI, float x, float y);
	void SpriteSetColor(int selfKI, uint8_t r, uint8_t g, uint8_t b);
	void SpriteSetOpacity(int selfKI, uint8_t o);
	void SpriteSetVisible(int selfKI, bool b);
}
