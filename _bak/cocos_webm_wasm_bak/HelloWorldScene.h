#pragma once
#include "cocos2d.h"
#include "Wasm3.hpp"

class HelloWorld : public cocos2d::Scene {
public:
    virtual bool init() override;
    virtual void update(float delta) override;

    std::unique_ptr<Wasm3> w3;

};
