#include "xx2d.h"

namespace xx {

	Shared<Frame> MakeFrame(Shared<GLTexture> t) {
		auto f = xx::Make<Frame>();
		f->anchor = { 0.5, 0.5 };
		f->textureRotated = false;
		f->spriteSize = f->spriteSourceSize = { (float)std::get<1>(t->vs), (float)std::get<2>(t->vs) };
		f->spriteOffset = {};
		f->textureRect = { 0, 0, f->spriteSize.x, f->spriteSize.y };
		f->tex = std::move(t);
		return f;
	}

	AnimFrame& Anim::GetCurrentAnimFrame() const {
		return (AnimFrame&)afs[cursor];
	}

	void Anim::Step() {
		if (++cursor == afs.size()) {
			cursor = 0;
		}
	}

	bool Anim::Update(float const& delta) {
		auto bak = cursor;
		timePool += delta;
	LabBegin:
		auto&& af = afs[cursor];
		if (timePool >= af.durationSeconds) {
			timePool -= af.durationSeconds;
			Step();
			goto LabBegin;
		}
		return bak != cursor;
	}

}
