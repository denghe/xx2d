#pragma once
#include "xx2d.h"

namespace xx {

	struct Mv {
		uint8_t codecId = 0;	// 0: vp8   1: vp9 ( current support vp9 only )
		uint8_t hasAlpha = 0;
		uint16_t width = 0;
		uint16_t height = 0;
		float duration = 0;	// issue
		std::vector<uint32_t> lens;	// all frame data's len here. hasAlpha == 0: [rgb], ...   == 1: [rgb + a], ...
		xx::Data data;	// all frame data here

		std::vector<uint8_t*> bufs;	// fill by Load
		uint32_t count = 0;	// fill by Load

		// load data from .xxmv. return 0 mean success
		int Load(xx::Data_r d);

		operator bool() const;

		void Clear();

		using YuvaHandler = std::function<int(int const& frameIndex, uint32_t const& w, uint32_t const& h
			, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData
			, uint32_t const& yaStride, uint32_t const& uvStride)>;

		// decode & handle every frmae's yuva data
		int ForeachFrame(YuvaHandler const& h) const;

	protected:
		int GetFrameBuf(uint32_t const& idx, uint8_t const*& rgbBuf, uint32_t& rgbBufLen) const;
		int GetFrameBuf(uint32_t idx, uint8_t const*& rgbBuf, uint32_t& rgbBufLen, uint8_t const*& aBuf, uint32_t& aBufLen) const;
	};
}
