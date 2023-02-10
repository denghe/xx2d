#include "pch.h"
#include "logic.h"
#include "logic8.h"

void Logic8::Init(Logic* logic) {
	this->logic = logic;

	std::cout << "Logic8 Init( node tests )" << std::endl;

	t.Emplace(xx::engine.LoadTexture("res/mouse.pkm"));

	//mices.Init(this);

	ls.FillCirclePoints({}, 8, {}, 8);
	ls.SetColor({ 255,0,0,255 });
	ls.SetPosition({ -16 * 8, -16 * 8 });
	

	n.sprite.SetTexture(t);
	n.sprite.SetScale(8);

	{
		auto& c = n.children.emplace_back().Emplace();
		c->sprite.SetTexture(t);
		c->sprite.SetParentAffineTransform(&n.sprite.at);
		c->sprite.SetPosition({ 0,0 });

		c->border.FillBoxPoints({}, c->sprite.frame->spriteSize);
	}
	//{
	//	auto& c = n.children.emplace_back().Emplace();
	//	c->sprite.SetTexture(t);
	//	c->sprite.SetParentAffineTransform(&n.sprite.at);
	//	c->sprite.SetPosition({ -16,-16 });
	//}
	//{
	//	auto& c = n.children.emplace_back().Emplace();
	//	c->sprite.SetTexture(t);
	//	c->sprite.SetParentAffineTransform(&n.sprite.at);
	//	c->sprite.SetPosition({ 16,-16 });
	//}
	//{
	//	auto& c = n.children.emplace_back().Emplace();
	//	c->sprite.SetTexture(t);
	//	c->sprite.SetParentAffineTransform(&n.sprite.at);
	//	c->sprite.SetPosition({ 16,16 });
	//}
	//{
	//	auto& c = n.children.emplace_back().Emplace();
	//	c->sprite.SetTexture(t);
	//	c->sprite.SetParentAffineTransform(&n.sprite.at);
	//	c->sprite.SetPosition({ -16,16 });
	//}
}

int Logic8::Update() {

	//n.sprite.SetRotate(n.sprite.radians + 0.0001);
	n.sprite.Draw();
	for (auto& c : n.children) {
		c->sprite.dirtyParentAffineTransform = true;	// tell children: parent trans changed
		c->sprite.Draw();
	}
	ls.Draw();

	return 0;
}
