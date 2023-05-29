print("test3.lua begin")

-- init search path ( cpp )
xxSearchPathAdd("res", true)

-- sync search path to lua -> package.path
xxSearchPathSync()
print(package.path)


-- consts
M_PI = 3.14159265358979323846
frameDelaySeconds = 1 / 60


-- utils

-- for(kv : t) if (f(kv)) del(t)
function Foreach(t, f)
	local n = #t
	if n == 0 then return end
	for i = n, 1, -1 do
		if (f(t[i])) then
			table.remove(t, i)
		end
	end
end


-- Scene metatable
Scene = {}
Scene.__index = Scene
Scene.Make = function()
	local t = {}
	setmetatable(t, Scene)
	return t
end

Scene.Init = function(self)
	self.tex = xxLoadSharedTexture("res/mouse.pkm")
	self.ms = {}
	self.timePool = 0
	self.radians = 0
end

Scene.Update = function(self)
	local fds = frameDelaySeconds
	local tp = self.timePool + xxDelta()
	local ms = self.ms
	while tp >= fds do
		tp = tp - fds

		local r = self.radians
		for i = 1, 50 do
			r = r + 0.005

			local m = Mouse.Make()
			m:Init(self, 0, 0, r)
			table.insert(ms, m)

		end
		self.radians = r

		Foreach(ms, function(m)
			return m:Update()
		end)

	end
	self.timePool = tp

	Foreach(ms, function(m)
		m:Draw()
	end)
end


-- Mouse metatable
Mouse = {}
Mouse.__index = Mouse
Mouse.Make = function()
	local t = {}
	setmetatable(t, Mouse)
	return t
end

Mouse.Init = function(self, owner, posX, posY, radians, scale, colorR, colorG, colorB, colorA)
	scale = scale or 1
	colorR = colorR or 255
	colorG = colorG or 255
	colorB = colorB or 255
	colorA = colorA or 255
	local body = xxQuad()
		:SetTexture(owner.tex)
		:SetPosition(posX, posY)
		:SetScale(scale)
		:SetRotate(-radians + M_PI / 2)
		:SetColor(colorR, colorG, colorB, colorA)
	local r = body:GetRotate()
	self.body = body
	self.baseIncX = math.sin(r) * 2
	self.baseIncY = math.cos(r) * 2
end

Mouse.Update = function(self)
	local body = self.body
	body:AddPosition(self.baseIncX, self.baseIncY)
	local posX, posY = body:GetPosition()
	if (posX * posX > (1800 / 2) * (1800 / 2)
		or posY * posY > (1000 / 2) * (1000 / 2)) then
		return true
	end
end

Mouse.Draw = function(self)
	local body = self.body
	local r,g,b,a = body:GetColor()	-- bak
	body:AddPosition(3, 3):SetColor(255,127,127,127):Draw()	-- shadow
	body:AddPosition(-3, -3):SetColor(r,g,b,a):Draw()
end



-- logic
local fnt = xxLoadBMFont("res/font/3500+.fnt")
local fpsViewer = xxFpsViewer()
local scene = Scene.Make()
scene:Init()

function Update()
	scene:Update()
	fpsViewer:Draw(fnt)
end


print("test3.lua end")

--[[
local lbl = xxSimpleLabel()
print(lbl)
lbl:SetText(fnt, "hello")

lbl:Draw()
]]



--[[

cocos test code backup:


AppDelegate.cpp:

        glView = GLViewImpl::createWithRect(
            "HelloCpp", ax::Rect(0, 0, 1800, 1000));
    // Set the design resolution
    glView->setDesignResolutionSize(1800, 1000, ResolutionPolicy::NO_BORDER);


HelloWorldScene.h:

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "axmol.h"
#include "xx_helpers.h"
#include "xx_listlink.h"

class HelloWorld;
struct Mouse {
    std::shared_ptr<ax::Sprite> body, shadow;
	ax::Vec2 baseInc{};
    void Init(HelloWorld* owner,
              ax::Vec2 const& pos,
              float radians,
              float const& scale     = 1,
              ax::Color4B const& color = {255, 255, 255, 255});
	int Update();
};

class HelloWorld : public ax::Scene
{
public:
    virtual bool init() override;

    // a selector callback
    void menuCloseCallback(Ref* sender);

    virtual void update(float delta) override;

    ax::Node* container;
    ax::Texture2D* tex;
    xx::ListLink<Mouse> ms;
    float timePool{}, radians{};
};

#endif  // __HELLOWORLD_SCENE_H__



HelloWorldScene.cpp:

#include "HelloWorldScene.h"

bool HelloWorld::init() {
    if (!Scene::init()) return false;

    container = ax::Node::create();
    container->setPosition({1800 / 2, 1000 / 2});
    this->addChild(container);

    tex = ax::Director::getInstance()->getTextureCache()->addImage("mouse.pkm");
    scheduleUpdate();
    return true;
}

void HelloWorld::update(float delta) {
    timePool += delta;
    while (timePool >= 1.f / 60) {
        timePool -= 1.f / 60;

        for (size_t i = 0; i < 50; i++) {
            radians += 0.005f;
            ms.Emplace().Init(this, {}, radians);
        }

        ms.Foreach([](auto& m) -> bool { return m.Update(); });
    }
}

void SpriteRmoveFunc(ax::Sprite* s) {
    s->removeFromParentAndCleanup(true);
}

void Mouse::Init(HelloWorld* owner,
                 ax::Vec2 const& pos,
                 float radians,
                 float const& scale,
                 ax::Color4B const& color)
{
    auto angle = (radians - M_PI / 2) * 180.f / M_PI;

    auto s = ax::Sprite::createWithTexture(owner->tex);
    s->setPosition(pos + ax::Vec2{3, 3});
    s->setScale(scale);
    s->setRotation(angle);
    s->setColor({255, 127, 127});
    s->setOpacity(127);
    s->setGlobalZOrder(-radians);
    owner->container->addChild(s);
    shadow = std::shared_ptr<ax::Sprite>(s, SpriteRmoveFunc);

    s = ax::Sprite::createWithTexture(owner->tex);
    s->setPosition(pos);
    s->setScale(scale);
    s->setRotation(angle);
    s->setColor({color.r, color.g, color.b});
    s->setOpacity(color.a);
    s->setGlobalZOrder(-radians - 0.001f);
    owner->container->addChild(s);
    body = std::shared_ptr<ax::Sprite>(s, SpriteRmoveFunc);

    radians += M_PI / 2;
    baseInc = {std::sin(radians), std::cos(radians)};
    baseInc *= 2;
}

int Mouse::Update()
{
    auto pos = body->getPosition() + baseInc;
    body->setPosition(pos);
    if (pos.x * pos.x > (1800 / 2) * (1800 / 2) || pos.y * pos.y > (1000 / 2) * (1000 / 2))
        return 1;
    shadow->setPosition(pos + ax::Vec2{3, 3});

    return 0;
}


]]
