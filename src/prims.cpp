#pragma once
#include "pch.h"
#include <zstd.h>

namespace xx {

	void ZstdDecompress(std::string_view const& src, xx::Data& dst) {
		auto&& siz = ZSTD_getFrameContentSize(src.data(), src.size());
		if (ZSTD_CONTENTSIZE_UNKNOWN == siz) throw std::logic_error("ZstdDecompress error: unknown content size.");
		if (ZSTD_CONTENTSIZE_ERROR == siz) throw std::logic_error("ZstdDecompress read content size error.");
		dst.Resize(siz);
		if (0 == siz) return;
		siz = ZSTD_decompress(dst.buf, siz, src.data(), src.size());
		if (ZSTD_isError(siz)) throw std::logic_error("ZstdDecompress decompress error.");
		dst.Resize(siz);
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
