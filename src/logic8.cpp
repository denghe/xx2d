#include "pch.h"
#include "logic.h"
#include "logic8.h"

void Logic8::Init(Logic* logic) {
	this->logic = logic;
	std::cout << "Logic8 Init( node tests )" << std::endl;

	ls.FillCirclePoints({}, 8, {}, 8)
		.SetColor({ 255,0,0,255 })
		.SetPosition({ -16 * 8, -16 * 8 });
	
	auto tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");
	node.sprite.SetTexture(tex)
		.SetScale(8);

	node.children.emplace_back().Emplace()->sprite.SetTexture(tex)
		.SetParentAffineTransform(&node.sprite.at)					// store parent trans ptr
		.SetPosition({ 0,0 });
}

int Logic8::Update() {

	node.sprite.SetRotate(node.sprite.radians + 0.0001).Draw();
	for (auto& c : node.children) {
		c->sprite.dirtyParentAffineTransform = true;				// tell children: parent trans changed
		c->sprite.Draw();
	}
	ls.Draw();

	return 0;
}










//c->border.FillBoxPoints({}, c->sprite.Size());
