#pragma once
#include "Env.h"
#include "CCRefPtr.h"
#include "xx_dict.h"
#include "xx_xxmv_cocos.h"

inline cocos2d::Director* __director = nullptr;
inline cocos2d::Scene* __scene = nullptr;

inline size_t __wasmBaseMemory = 0;

inline int64_t __keyId = 0;
inline xx::Dict<int64_t, cocos2d::RefPtr<cocos2d::Sprite>> __sprites;

inline xx::XxmvCocos __mv;

void EnvInit(void* scene) {
	__director = cocos2d::Director::getInstance();
	__scene = (cocos2d::Scene*)scene;
}

extern "C" {
	int XxmvLoad(char* fileName) {
		if (int r = __mv.LoadFromXxmv(std::string(fileName) + ".xxmv")) return -r;
		if (int r = __mv.FillToSpriteFrameCache(fileName, "_", "")) return -r;
		return (int)__mv.count;
	}

	int SpriteNew(char* spriteFrameName, int parentIndex) {
		spriteFrameName += __wasmBaseMemory;
		auto spr = cocos2d::Sprite::createWithSpriteFrameName(spriteFrameName);
		auto r = __sprites.Add(++__keyId, spr);
		assert(r.success);
		if (parentIndex == -1) {
			__scene->addChild(spr);
		}
		else {
			assert(__sprites.IndexExists(parentIndex));
			__sprites.ValueAt(parentIndex)->addChild(spr);
		}
		return r.index;
	}

	void SpriteDelete(int index) {
		assert(__sprites.IndexExists(index));
		__sprites.ValueAt(index)->removeFromParent();
		__sprites.RemoveAt(index);
	}

	void SpriteSetPosition(int index, float x, float y) {
		assert(__sprites.IndexExists(index));
		__sprites.ValueAt(index)->setPosition(x, y);
	}

	void SpriteSetZOrder(int index, float z) {
		assert(__sprites.IndexExists(index));
		__sprites.ValueAt(index)->setGlobalZOrder(z);
	}

	void SpriteSetAnchor(int index, float x, float y) {
		assert(__sprites.IndexExists(index));
		__sprites.ValueAt(index)->setAnchorPoint({ x, y });
	}

	void SpriteSetRotation(int index, float r) {
		assert(__sprites.IndexExists(index));
		__sprites.ValueAt(index)->setRotation(r);
	}

	void SpriteSetColor(int index, uint8_t r, uint8_t g, uint8_t b) {
		assert(__sprites.IndexExists(index));
		__sprites.ValueAt(index)->setColor({ r,g,b });
	}

	void SpriteSetOpacity(int index, uint8_t o) {
		assert(__sprites.IndexExists(index));
		__sprites.ValueAt(index)->setOpacity(o);
	}

	void SpriteSetVisible(int index, bool b) {
		assert(__sprites.IndexExists(index));
		__sprites.ValueAt(index)->setVisible(b);
	}

	void SpriteSetFrameName(int index, char* spriteFrameName) {
		assert(__sprites.IndexExists(index));
		__sprites.ValueAt(index)->setSpriteFrame(spriteFrameName);
	}
}


//wasm3::environment w3;
//wasm3::runtime runtime = w3.new_runtime(1024);
	//template<bool FromWasm = false>
	//if constexpr (FromWasm) fileName += wasmBaseMemory;
	//template<bool FromWasm = false>

