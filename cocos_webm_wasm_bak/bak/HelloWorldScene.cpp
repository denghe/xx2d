#include "HelloWorldScene.h"
#include "xx_xxmv_cocos.h"
#include "xx_helpers.h"
#include "Env.h"
#include "Logic.h"

bool HelloWorld::init() {
    if (!Scene::init()) return false;
    scheduleUpdate();

    EnvInit(this);

    this->logic = LogicNew();
    this->logicInBuf = LogicGetInBuf(this->logic);
    this->logicOutBuf = LogicGetOutBuf(this->logic);

    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](cocos2d::Touch* t, cocos2d::Event* e)->bool {
        auto result = touches.Add(t, logic);
        assert(result.success);
        auto p = t->getLocation();
        return LogicTouchBegan(logic, result.index, p.x, p.y);
    };
    listener->onTouchMoved = [this](cocos2d::Touch* t, cocos2d::Event* e)->void {
        auto iter = touches.Find(t);
        assert(iter != -1);
        auto p = t->getLocation();
        LogicTouchMoved(touches.ValueAt(iter), iter, p.x, p.y);
    };
    listener->onTouchEnded = [this](cocos2d::Touch* t, cocos2d::Event* e)->void {
        auto iter = touches.Find(t);
        assert(iter != -1);
        auto p = t->getLocation();
        LogicTouchEnded(touches.ValueAt(iter), iter, p.x, p.y);
        touches.RemoveAt(iter);
    };
    listener->onTouchCancelled = [this](cocos2d::Touch* t, cocos2d::Event* e)->void {
        auto iter = touches.Find(t);
        assert(iter != -1);
        LogicTouchCancelled(touches.ValueAt(iter), iter);
        touches.RemoveAt(iter);
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void HelloWorld::update(float delta) {
    LogicUpdate(logic, delta);
}


















//xx::XxmvCocos mv;
//auto GetAnim = [&](std::string const& fileName, float frameDelay = 1.f / 60)-> cocos2d::Animation* {
//    int r = mv.LoadFromXxmv(fileName + ".xxmv");
//    assert(!r);
//    r = mv.FillToSpriteFrameCache(fileName, "_", "");
//    assert(!r);

//    cocos2d::Vector<cocos2d::SpriteFrame*> sfs;
//    auto sfc = cocos2d::SpriteFrameCache::getInstance();
//    for (int i = 1; i <= mv.count; ++i) {
//        auto sf = sfc->getSpriteFrameByName(fileName + "_" + std::to_string(i));
//        assert(sf);
//        sfs.pushBack(sf);
//    }
//    return cocos2d::Animation::createWithSpriteFrames(sfs, frameDelay);
//};

//auto GenSpr = [&](std::string_view const& name, int fps, float x, float y)->cocos2d::Sprite* {
//    auto sprite = cocos2d::Sprite::create();
//    sprite->setPosition(x, y);
//    this->addChild(sprite, 0);
//    sprite->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(GetAnim(std::string(name), 1.f / fps))));
//    return sprite;
//};

//float x = 100, y = 100, w = 200;
//GenSpr("st_m_20", 30, x, y); x += w;
//GenSpr("st_m_50", 30, x, y); x += w;
//GenSpr("st_m_100", 30, x, y); x += w;
//GenSpr("st_m_200", 30, x, y); x += w;

//GenSpr("st_d_20", 30, x, y); x += w;
//GenSpr("st_d_50", 30, x, y); x += w;
//GenSpr("st_d_100", 30, x, y); x += w;
//GenSpr("st_d_200", 30, x, y); x += w;

//x = 100; y += 200; w = 180;
//GenSpr("st_l_20", 30, x, y); x += w;
//GenSpr("st_l_50", 30, x, y); x += w;
//GenSpr("st_l_100", 30, x, y); x += w;
//GenSpr("st_l_200", 30, x, y); x += w;

//x = 270; y += 400; w = 380;
//GenSpr("lxjj_m_20", 30, x, y); x += w;
//GenSpr("lxjj_m_50", 30, x, y); x += w;
//GenSpr("lxjj_m_100", 30, x, y); x += w;
//GenSpr("lxjj_m_200", 30, x, y); x += w;

//GenSpr("fd_m_200", 60, 1200, 300);
