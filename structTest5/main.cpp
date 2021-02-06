#include "xx_webm.h"

int main(int argc, char* argv[]) {
	(void)argc;	(void)argv;

	xx::Webm wm;
	if (int r = wm.LoadFromWebm("res/a.webm")) return r;
	if (int r = wm.SaveToXxmv("res/a.xxmv")) return r;

	if (int r = wm.LoadFromXxmv("res/a.xxmv")) return r;
	//if (int r = wm.SaveToPngs("res/", "a")) return r;

	// 性能评测结果：下面这行代码 9900k cpu 大约执行 0.17x 秒. 如果不写磁盘 0.04x 秒. 手机上性能如何，需要实测。有可能慢 10 倍+ ?
	if (int r = wm.SaveToPackedPngs("res/", "a")) return r;

	std::cout << "done." << std::endl;
	return 0;
}

/*

#include "HelloWorldScene.h"
#include "xx_webm.h"
#include "ui/CocosGUI.h"

cocos2d::ui::Button* CreateButton(cocos2d::Vec2 const& pos, std::string const& txt, cocos2d::ccMenuCallback&& callback, cocos2d::Node* const& container) {
	auto b = cocos2d::ui::Button::create();
	if (!b) return b;
	b->setTitleColor(cocos2d::Color3B::BLUE);
	b->setTitleFontName("arial");
	b->setTitleFontSize(32);
	b->setTitleText(txt);
	b->setAnchorPoint({ 0.5, 0.5 });
	b->setPosition(pos);
	b->setZoomScale(0.1f);
	b->setPressedActionEnabled(true);
	b->addClickEventListener(std::move(callback));
	container->addChild(b);
	return b;
}

bool HelloWorld::init() {
	if (!Scene::init()) return false;

	auto director = cocos2d::Director::getInstance();
	auto fs = cocos2d::FileUtils::getInstance();
	auto sfc = cocos2d::SpriteFrameCache::getInstance();

	auto visibleSize = director->getVisibleSize();
	auto origin = director->getVisibleOrigin();

	std::string out;

	auto secs = xx::NowEpochSeconds();
	xx::Webm wm;
	int r = wm.LoadFromWebm(fs->fullPathForFilename("zhangyu.webm"));
	if (r) throw r;
	r = wm.FillToSpriteFrameCache("a");
	if (r) {
		std::cout << "r=" << r << std::endl;
		throw r;
	}
	auto secs2 = xx::NowEpochSeconds();
	out.append(std::to_string(secs2 - secs));
	out.append("  ");
	out.append(std::to_string(wm.count));


	auto anim = cocos2d::Animation::create();
	if (!anim) throw - 1;
	for (int i = 1; i <= wm.count; ++i) {
		auto sf = sfc->getSpriteFrameByName(std::string("a (") + std::to_string(i) + ").png");
		if (!sf) throw - 2;
		anim->addSpriteFrame(sf);
	}
	anim->setDelayPerUnit(1.f / 60);
	anim->setLoops(-1);

	auto sprite = cocos2d::Sprite::create();
	sprite->setPosition(cocos2d::Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(sprite);

	auto animate = cocos2d::Animate::create(anim);
	if (!animate) throw - 3;
	sprite->runAction(animate);

	auto lbl = cocos2d::Label::createWithSystemFont(out, "", 32);
	lbl->setPosition(cocos2d::Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 200));
	lbl->setAnchorPoint({ 0.5f, 0.5f });
	this->addChild(lbl);

	CreateButton({ visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 200 }
		, "remove sprite",
		[sprite, sfc](Ref* ref) {
			sprite->removeFromParent();
			sfc->removeUnusedSpriteFrames();
			if (sfc->getSpriteFrameByName(std::string("a (1).png"))) throw - 4;
			((Node*)ref)->removeFromParent();
		}, this);

	return true;
}


*/