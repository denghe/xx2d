#pragma once
#include <cstdint>
// call by host
void EnvInit(void* scene);

extern "C" {
	/*********************************************************************/

	// load .xxmv file by fileName, return key index
	extern int XxmvNew(char const* fileName, int fileNameSize);

	// unload .xxmv sprite frames by key index
	extern void XxmvDelete(int selfKI);

	// return sprite frames .size()
	extern int XxmvGetFramesCount(int selfKI);

	/*********************************************************************/

	// scene->addChild( create sprite ), return key index
	extern int SpriteNew(int xxmvKI, int parentIndex);

	// remove sprite from scene childs by key index
	extern void SpriteDelete(int selfKI);

	// set sprite's sprite frame by xxmv's sprite frames[ index ]
	extern void SpriteSetXxmvFrame(int selfKI, int xxmvKI, int index);

	extern void SpriteSetPosition(int selfKI, float x, float y);
	extern void SpriteSetZOrder(int selfKI, float z);
	extern void SpriteSetAnchor(int selfKI, float x, float y);
	extern void SpriteSetRotation(int selfKI, float r);
	extern void SpriteSetScale(int selfKI, float x, float y);
	extern void SpriteSetColor(int selfKI, uint8_t r, uint8_t g, uint8_t b);
	extern void SpriteSetOpacity(int selfKI, uint8_t o);
	extern void SpriteSetVisible(int selfKI, bool b);
}
