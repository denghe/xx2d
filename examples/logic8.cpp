#include "xx2d_pch.h"
#include "game_looper.h"
#include "logic8.h"

void Logic8::Init(GameLooper* looper) {
	this->looper = looper;
	std::cout << "Logic8 Init( node tests )" << std::endl;

	auto tex = xx::engine.LoadTextureFromCache("res/mouse.pkm");

	node.SetTexture(tex).SetScale(8);

	auto& c1 = node.children.emplace_back().Emplace();
	c1->SetTexture(tex).SetScale(0.5).SetColor({ 255,0,0,255 }).SetPosition({ 16, 16 });

	auto& c2 = c1->children.emplace_back().Emplace();
	c2->SetTexture(tex).SetScale(0.25).SetColor({ 0,255,0,255 }).SetPosition({ 16, 16 });

	auto& c3 = c2->children.emplace_back().Emplace();
	c3->SetTexture(tex).SetScale(0.125).SetColor({ 0,0,255,255 }).SetPosition({ 16, 16 });

	node.FillParentAffineTransaction();
}

int Logic8::Update() {

	node.AddRotate(0.00001);
	node[0]->AddRotate(-0.0001);
	node[0][0]->AddRotate(0.0001);
	node[0][0][0]->AddRotate(-0.0001);
	node.Draw();

	return 0;
}
