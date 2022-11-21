#include "cocos2d.h"
#include "CCRefPtr.h"
#include "xx_dict.h"
#include "xx_podpool.h"
#include "xx_xxmv_cocos.h"
#include "wasm3_cpp.h"
#include "Env.h"

struct Wasm3 {
	wasm3::environment env;
	wasm3::runtime rt = env.new_runtime(1024 * 1024);
	wasm3::module mod;

	std::unique_ptr<wasm3::function> LogicNew;
	std::unique_ptr<wasm3::function> LogicDelete;
	std::unique_ptr<wasm3::function> LogicGetInBuf;
	std::unique_ptr<wasm3::function> LogicGetOutBuf;
	std::unique_ptr<wasm3::function> LogicTouchBegan;
	std::unique_ptr<wasm3::function> LogicTouchMoved;
	std::unique_ptr<wasm3::function> LogicTouchEnded;
	std::unique_ptr<wasm3::function> LogicTouchCancelled;
	std::unique_ptr<wasm3::function> LogicUpdate;


	inline static cocos2d::Director* __director = nullptr;
	inline static cocos2d::Scene* __scene = nullptr;
	inline static cocos2d::SpriteFrameCache* __sfcache = nullptr;
	inline static size_t __wasmBaseMemory = 0;

	inline static xx::PodPool<cocos2d::RefPtr<cocos2d::Sprite>> __sprites;
	inline static xx::Dict<std::string, std::vector<cocos2d::RefPtr<cocos2d::SpriteFrame>>> __spriteFramess;
	inline static xx::XxmvCocos __mv;

	cocos2d::EventListenerTouchOneByOne* listener = nullptr;
	cocos2d::Scene* scene = nullptr;
	xx::Dict<cocos2d::Touch*, void*> touches;
	void* logic = nullptr, * logicInBuf = nullptr, * logicOutBuf = nullptr;

	Wasm3(cocos2d::Scene* scene, cocos2d::Data const& d)
		: mod(env.parse_module(d.getBytes(), d.getSize()))
		, scene(scene)
	{
		rt.load(mod);

		__wasmBaseMemory = (size_t)rt.get_base_addr();
		__director = cocos2d::Director::getInstance();
		__scene = (cocos2d::Scene*)scene;
		__sfcache = cocos2d::SpriteFrameCache::getInstance();

		mod.link_optional("*", "ConsoleLog", ConsoleLog);
		mod.link_optional("*", "GetNow", GetNow);
		mod.link_optional("*", "GetSteadyNow", GetSteadyNow);

		mod.link_optional("*", "XxmvNew", XxmvNew);
		mod.link_optional("*", "XxmvDelete", XxmvDelete);
		mod.link_optional("*", "XxmvGetFramesCount", XxmvGetFramesCount);

		mod.link_optional("*", "SpriteNew", SpriteNew);
		mod.link_optional("*", "SpriteDelete", SpriteDelete);
		mod.link_optional("*", "SpriteSetXxmvFrame", SpriteSetXxmvFrame);
		mod.link_optional("*", "SpriteSetPosition", SpriteSetPosition);
		mod.link_optional("*", "SpriteSetZOrder", SpriteSetZOrder);
		mod.link_optional("*", "SpriteSetAnchor", SpriteSetAnchor);
		mod.link_optional("*", "SpriteSetRotation", SpriteSetRotation);
		mod.link_optional("*", "SpriteSetScale", SpriteSetScale);
		mod.link_optional("*", "SpriteSetColor", SpriteSetColor);
		mod.link_optional("*", "SpriteSetOpacity", SpriteSetOpacity);
		mod.link_optional("*", "SpriteSetVisible", SpriteSetVisible);

		LogicNew = std::make_unique<wasm3::function>(rt.find_function("LogicNew"));
		LogicDelete = std::make_unique<wasm3::function>(rt.find_function("LogicDelete"));
		LogicGetInBuf = std::make_unique<wasm3::function>(rt.find_function("LogicGetInBuf"));
		LogicGetOutBuf = std::make_unique<wasm3::function>(rt.find_function("LogicGetOutBuf"));
		LogicTouchBegan = std::make_unique<wasm3::function>(rt.find_function("LogicTouchBegan"));
		LogicTouchMoved = std::make_unique<wasm3::function>(rt.find_function("LogicTouchMoved"));
		LogicTouchEnded = std::make_unique<wasm3::function>(rt.find_function("LogicTouchEnded"));
		LogicTouchCancelled = std::make_unique<wasm3::function>(rt.find_function("LogicTouchCancelled"));
		LogicUpdate = std::make_unique<wasm3::function>(rt.find_function("LogicUpdate"));

		logic = (void*)(uint32_t)LogicNew->call<void*>();
		logicInBuf = rt.get_true_addr(LogicGetInBuf->call<void*>(this->logic));
		logicOutBuf = rt.get_true_addr(LogicGetOutBuf->call<void*>(this->logic));

		listener = cocos2d::EventListenerTouchOneByOne::create();
		listener->onTouchBegan = [this](cocos2d::Touch* t, cocos2d::Event* e)->bool {
			auto result = touches.Add(t, logic);
			assert(result.success);
			auto p = t->getLocation();
			return LogicTouchBegan->call<int>(logic, result.index, p.x, p.y) == 1;
		};
		listener->onTouchMoved = [this](cocos2d::Touch* t, cocos2d::Event* e)->void {
			auto iter = touches.Find(t);
			assert(iter != -1);
			auto p = t->getLocation();
			LogicTouchMoved->call(touches.ValueAt(iter), iter, p.x, p.y);
		};
		listener->onTouchEnded = [this](cocos2d::Touch* t, cocos2d::Event* e)->void {
			auto iter = touches.Find(t);
			assert(iter != -1);
			auto p = t->getLocation();
			LogicTouchEnded->call(touches.ValueAt(iter), iter, p.x, p.y);
			touches.RemoveAt(iter);
		};
		listener->onTouchCancelled = [this](cocos2d::Touch* t, cocos2d::Event* e)->void {
			auto iter = touches.Find(t);
			assert(iter != -1);
			LogicTouchCancelled->call(touches.ValueAt(iter), iter);
			touches.RemoveAt(iter);
		};
		scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, scene);
	}

	void Update(float delta) {
		LogicUpdate->call(logic, delta);
	}

	~Wasm3() {
		LogicDelete->call(logic);
		scene->getEventDispatcher()->removeEventListener(listener);
		scene->unscheduleUpdate();
	}

	inline static void FixAddress(void*& p) {
		if constexpr (sizeof(void*) > 4) {
			if (((size_t)p & 0xFFFFFFFF00000000u) == 0xCCCCCCCC00000000u) {
				p = (void*)((uint32_t)p + __wasmBaseMemory);
			}
		}
	}
};





extern "C" {

	inline void ConsoleLog(void* str) {
		Wasm3::FixAddress(str);
		cocos2d::log("%s", str);
	}

	inline long long GetNow() {
		return (int64_t)std::chrono::system_clock::now().time_since_epoch().count();
	}

	inline long long GetSteadyNow() {
		return std::chrono::steady_clock::now().time_since_epoch().count();
	}

	/*********************************************************************/

	inline int XxmvNew(void* fileName, int fileNameSize) {
		Wasm3::FixAddress(fileName);
		std::string fn((char*)fileName, fileNameSize);

		auto iter = Wasm3::__spriteFramess.Find(fn);
		if (iter != -1) return -iter;

		if (int r = Wasm3::__mv.LoadFromXxmv(fn + ".xxmv")) return -r;
		if (int r = Wasm3::__mv.FillToSpriteFrameCache(fn, "_", "")) return -r;

		std::vector<cocos2d::RefPtr<cocos2d::SpriteFrame>> sfs;
		fn.append("_");
		for (int i = 0; i < (int)Wasm3::__mv.count; i++) {
			auto sf = Wasm3::__sfcache->getSpriteFrameByName(fn + std::to_string(i));
			assert(sf);
			sfs.push_back(sf);
		}
		fn.pop_back();	// remove _
		auto result = Wasm3::__spriteFramess.Add(std::move(fn), std::move(sfs));
		assert(result.success);
		return result.index;
	}

	inline void XxmvDelete(int selfKI) {
		assert(Wasm3::__spriteFramess.IndexExists(selfKI));

		auto fn = Wasm3::__spriteFramess.KeyAt(selfKI);
		auto n = (int)Wasm3::__spriteFramess.ValueAt(selfKI).size();
		Wasm3::__spriteFramess.RemoveAt(selfKI);
		fn.append("_");
		for (int i = 0; i < n; i++) {
			Wasm3::__sfcache->removeSpriteFrameByName(fn + std::to_string(i));
		}
	}

	inline int XxmvGetFramesCount(int selfKI) {
		assert(Wasm3::__spriteFramess.IndexExists(selfKI));
		return (int)Wasm3::__spriteFramess.ValueAt(selfKI).size();
	}

	/*********************************************************************/

	inline int SpriteNew(int xxmvKI, int parentIndex) {
		assert(Wasm3::__spriteFramess.IndexExists(xxmvKI));
		assert(Wasm3::__spriteFramess.ValueAt(xxmvKI).size() > 0);

		auto spr = cocos2d::Sprite::createWithSpriteFrame(Wasm3::__spriteFramess.ValueAt(xxmvKI)[0]);
		if (parentIndex == -1) {
			Wasm3::__scene->addChild(spr);
		}
		else {
			Wasm3::__sprites[parentIndex]->addChild(spr);
		}
		return Wasm3::__sprites.Add(spr);
	}

	inline void SpriteDelete(int selfKI) {
		Wasm3::__sprites[selfKI]->removeFromParent();
		Wasm3::__sprites.Remove(selfKI);
	}

	inline void SpriteSetPosition(int selfKI, float x, float y) {
		Wasm3::__sprites[selfKI]->setPosition(x, y);
	}

	inline void SpriteSetZOrder(int selfKI, float z) {
		Wasm3::__sprites[selfKI]->setGlobalZOrder(z);
	}

	inline void SpriteSetAnchor(int selfKI, float x, float y) {
		Wasm3::__sprites[selfKI]->setAnchorPoint({ x, y });
	}

	inline void SpriteSetRotation(int selfKI, float r) {
		Wasm3::__sprites[selfKI]->setRotation(r);
	}

	inline void SpriteSetScale(int selfKI, float x, float y) {
		Wasm3::__sprites[selfKI]->setScale(x, y);
	}

	inline void SpriteSetColor(int selfKI, int r, int g, int b) {
		Wasm3::__sprites[selfKI]->setColor({ (uint8_t)r, (uint8_t)g, (uint8_t)b });
	}

	inline void SpriteSetOpacity(int selfKI, int o) {
		Wasm3::__sprites[selfKI]->setOpacity(o);
	}

	inline void SpriteSetVisible(int selfKI, int b) {
		Wasm3::__sprites[selfKI]->setVisible(b);
	}

	inline void SpriteSetXxmvFrame(int selfKI, int xxmvKI, int index) {
		assert(Wasm3::__spriteFramess.IndexExists(xxmvKI));
		assert(Wasm3::__spriteFramess.ValueAt(xxmvKI).size() > index && index >= 0);
		Wasm3::__sprites[selfKI]->setSpriteFrame(Wasm3::__spriteFramess.ValueAt(xxmvKI)[index]);
	}
}
