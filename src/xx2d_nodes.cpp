#include "xx2d_pch.h"
#include "xx2d_nodes.h"

namespace xx {

	SpriteNode::Child& SpriteNode::operator[](size_t const& idx) {
		return children[idx];
	}

	void SpriteNode::FillParentAffineTransaction() {
		FillParentAffineTransaction(children, &at);
	}

	void SpriteNode::FillParentAffineTransaction(Children& cs, xx::AffineTransform* pat) {
		for (auto& c : cs) {
			c->SetParentAffineTransform(pat);
			c->FillParentAffineTransaction(c->children, c->pat);	// store parent trans ptr
		}
	}

	void SpriteNode::Draw() {
		xx::Sprite::Draw();
		Draw(children);
	}

	void SpriteNode::Draw(Children& cs) {
		for (auto& c : cs) {
			c->SubDraw();										// concat parent trans & force commit & draw
		}
		for (auto& c : cs) {
			c->Draw(c->children);
		}
	}

}
