#include "HelloWorldScene.h"
#include "xx_xxmv_cocos.h"
#include "xx_helpers.h"
#include "wasm3_cpp.h"
#include "Env.h"
#include "Logic.h"

bool HelloWorld::init() {
    if (!Scene::init()) return false;
    scheduleUpdate();

    EnvInit(this);
    LogicInit();

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


    return true;
}

void HelloWorld::update(float delta) {
    LogicUpdate(delta);
}
