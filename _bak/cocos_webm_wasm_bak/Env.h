#pragma once
#ifndef EMSCRIPTEN
// call by host
void EnvInit(void* scene, size_t EnvInit);
#endif

extern "C" {
	// print str to console
	extern void ConsoleLog(void* str);

	// return std::chrono::system_clock::now();
	extern long long GetNow();

	// return std::chrono::steady_clock::now();
	extern long long GetSteadyNow();

	/*********************************************************************/

	// load .xxmv file by fileName, return key index
	extern int XxmvNew(void* fileName, int fileNameSize);

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
	extern void SpriteSetColor(int selfKI, int r, int g, int b);
	extern void SpriteSetOpacity(int selfKI, int o);
	extern void SpriteSetVisible(int selfKI, int b);
}
