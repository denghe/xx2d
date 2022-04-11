#pragma once
#include "cocos2d.h"
#include "xx_dict.h"

class HelloWorld : public cocos2d::Scene {
public:
    virtual bool init() override;
    virtual void update(float delta) override;

    xx::Dict<cocos2d::Touch*, void*> touches;
    void *logic = nullptr, *logicInBuf = nullptr, *logicOutBuf = nullptr;
};
