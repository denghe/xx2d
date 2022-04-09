#pragma once
#include "Env.h"
#include "CCRefPtr.h"
#include "xx_dict.h"
#include "xx_xxmv_cocos.h"

inline cocos2d::Director* __director = nullptr;
inline cocos2d::Scene* __scene = nullptr;
inline cocos2d::SpriteFrameCache* __sfcache = nullptr;

inline size_t __wasmBaseMemory = 0;

inline int64_t __keyId = 0;
inline xx::Dict<int64_t, cocos2d::RefPtr<cocos2d::Sprite>> __sprites;

inline xx::Dict<std::string, std::vector<cocos2d::RefPtr<cocos2d::SpriteFrame>>> __spriteFramess;

inline xx::XxmvCocos __mv;

void EnvInit(void* scene) {
	__director = cocos2d::Director::getInstance();
	__scene = (cocos2d::Scene*)scene;
	__sfcache = cocos2d::SpriteFrameCache::getInstance();
}

extern "C" {
	/*********************************************************************/

	int XxmvNew(char const* fileName, int fileNameSize) {
		fileName += __wasmBaseMemory;
		std::string fn(fileName, fileNameSize);

		auto iter = __spriteFramess.Find(fn);
		if (iter != -1) return -iter;

		if (int r = __mv.LoadFromXxmv(fn + ".xxmv")) return -r;
		if (int r = __mv.FillToSpriteFrameCache(fn, "_", "")) return -r;

		std::vector<cocos2d::RefPtr<cocos2d::SpriteFrame>> sfs;
		fn.append("_");
		for (int i = 0; i < (int)__mv.count; i++) {
			auto sf = __sfcache->getSpriteFrameByName(fn + std::to_string(i));
			assert(sf);
			sfs.push_back(sf);
		}
		auto result = __spriteFramess.Add(std::move(fn), std::move(sfs));
		assert(result.success);
		return result.index;
	}

	void XxmvDelete(int selfKI) {
		assert(__spriteFramess.IndexExists(selfKI));

		auto fn = __spriteFramess.KeyAt(selfKI);
		auto n = (int)__spriteFramess.ValueAt(selfKI).size();
		__spriteFramess.RemoveAt(selfKI);
		fn.append("_");
		for (int i = 0; i < n; i++) {
			__sfcache->removeSpriteFrameByName(fn + std::to_string(i));
		}
	}

	int XxmvGetFramesCount(int selfKI) {
		assert(__spriteFramess.IndexExists(selfKI));
		return (int)__spriteFramess.ValueAt(selfKI).size();
	}

	/*********************************************************************/

	int SpriteNew(int xxmvKI, int parentIndex) {
		assert(__spriteFramess.IndexExists(xxmvKI));
		assert(__spriteFramess.ValueAt(xxmvKI).size() > 0);

		auto spr = cocos2d::Sprite::createWithSpriteFrame(__spriteFramess.ValueAt(xxmvKI)[0]);
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

	void SpriteDelete(int selfKI) {
		assert(__sprites.IndexExists(selfKI));
		__sprites.ValueAt(selfKI)->removeFromParent();
		__sprites.RemoveAt(selfKI);
	}

	void SpriteSetPosition(int selfKI, float x, float y) {
		assert(__sprites.IndexExists(selfKI));
		__sprites.ValueAt(selfKI)->setPosition(x, y);
	}

	void SpriteSetZOrder(int selfKI, float z) {
		assert(__sprites.IndexExists(selfKI));
		__sprites.ValueAt(selfKI)->setGlobalZOrder(z);
	}

	void SpriteSetAnchor(int selfKI, float x, float y) {
		assert(__sprites.IndexExists(selfKI));
		__sprites.ValueAt(selfKI)->setAnchorPoint({ x, y });
	}

	void SpriteSetRotation(int selfKI, float r) {
		assert(__sprites.IndexExists(selfKI));
		__sprites.ValueAt(selfKI)->setRotation(r);
	}

	void SpriteSetScale(int selfKI, float x, float y) {
		assert(__sprites.IndexExists(selfKI));
		__sprites.ValueAt(selfKI)->setScale(x, y);
	}

	void SpriteSetColor(int selfKI, uint8_t r, uint8_t g, uint8_t b) {
		assert(__sprites.IndexExists(selfKI));
		__sprites.ValueAt(selfKI)->setColor({ r,g,b });
	}

	void SpriteSetOpacity(int selfKI, uint8_t o) {
		assert(__sprites.IndexExists(selfKI));
		__sprites.ValueAt(selfKI)->setOpacity(o);
	}

	void SpriteSetVisible(int selfKI, bool b) {
		assert(__sprites.IndexExists(selfKI));
		__sprites.ValueAt(selfKI)->setVisible(b);
	}

	void SpriteSetXxmvFrame(int selfKI, int xxmvKI, int index) {
		assert(__sprites.IndexExists(selfKI));
		assert(__spriteFramess.IndexExists(xxmvKI));
		assert(__spriteFramess.ValueAt(xxmvKI).size() > index && index >= 0);
		__sprites.ValueAt(selfKI)->setSpriteFrame(__spriteFramess.ValueAt(xxmvKI)[index]);
	}
}


//wasm3::environment w3;
//wasm3::runtime runtime = w3.new_runtime(1024);
	//template<bool FromWasm = false>
	//if constexpr (FromWasm) fileName += wasmBaseMemory;
	//template<bool FromWasm = false>

