#include "xx2d_pch.h"

namespace xx {

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
