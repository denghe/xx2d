#pragma once
#include "main.h"
#include "vpx_decoder.h"
#include "vp8dx.h"

namespace xx {

	struct Xxmv {
		uint8_t codecId = 0;	// 0: vp8   1: vp9 ( current support vp9 only )
		uint8_t hasAlpha = 0;
		uint16_t width = 0;
		uint16_t height = 0;
		float duration = 0;	// issue
		std::vector<uint32_t> lens;	// all frame data's len here. hasAlpha == 0: [rgb], ...   == 1: [rgb + a], ...
		xx::Data data;	// all frame data here

		std::vector<uint8_t*> bufs;
		uint32_t count = 0;

		// load data from .xxmv. return 0 mean success
		inline int Load(xx::Data_r d) {
			Clear();
			// todo: verify header? version number?
			if (int r = d.ReadFixed(codecId)) return __LINE__;
			if (int r = d.ReadFixed(hasAlpha)) return __LINE__;
			if (int r = d.ReadFixed(width)) return __LINE__;
			if (int r = d.ReadFixed(height)) return __LINE__;
			if (int r = d.ReadFixed(duration)) return __LINE__;
			uint32_t siz;
			if (int r = d.ReadFixed(siz)) return r;
			if (d.offset + siz * sizeof(uint32_t) > d.len) return __LINE__;
			lens.resize(siz);
			if (int r = d.ReadBuf(lens.data(), siz * sizeof(uint32_t))) return __LINE__;
			if (int r = d.ReadFixed(siz)) return __LINE__;
			if (d.offset + siz > d.len) return __LINE__;
			data.Resize(siz);
			if (int r = d.ReadBuf(data.buf, data.len)) return __LINE__;


			count = (uint32_t)(hasAlpha ? lens.size() / 2 : lens.size());
			bufs.resize(lens.size());
			auto baseBuf = data.buf;
			for (int i = 0; i < lens.size(); ++i) {
				bufs[i] = (uint8_t*)baseBuf;
				baseBuf += lens[i];
			}
			if (baseBuf != data.buf + data.len) return __LINE__;
			return 0;
		}


		inline operator bool() const {
			return count != 0;
		}

		inline void Clear() {
			codecId = 0;
			hasAlpha = 0;
			width = 0;
			height = 0;
			duration = 0;
			lens.clear();
			data.Clear();

			bufs.clear();
			count = 0;
		}

		inline int GetFrameBuf(uint32_t const& idx, uint8_t const*& rgbBuf, uint32_t& rgbBufLen) {
			if (hasAlpha) return __LINE__;
			if (idx >= count) return __LINE__;
			rgbBuf = bufs[idx];
			rgbBufLen = lens[idx];
			return 0;
		}

		inline int GetFrameBuf(uint32_t idx, uint8_t const*& rgbBuf, uint32_t& rgbBufLen, uint8_t const*& aBuf, uint32_t& aBufLen) {
			if (!hasAlpha) return __LINE__;
			if (idx >= count) return __LINE__;
			idx *= 2;
			rgbBuf = bufs[idx];
			aBuf = bufs[idx + 1];
			rgbBufLen = lens[idx];
			aBufLen = lens[idx + 1];
			return 0;
		}

		//inline static int YuvaHandler(int const& frameIndex
		//	, uint32_t const& w, uint32_t const& h
		//	, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData
		//	, uint32_t const& yaStride, uint32_t const& uvStride) {
		// std::vector<uint8_t> bytes;
		//	bytes.resize(w * h * 4);
		//	return libyuv::I420AlphaToABGR(yData, yaStride, uData, uvStride, vData, uvStride, aData, yaStride, bytes.data(), w * 4, w, h, 0);
		//}


		// example:
		// r = mv.ForeachFrame([&](int const& frameIndex, uint32_t const& w, uint32_t const& h, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData, uint32_t const& yaStride, uint32_t const& uvStride)->int {
		//	// todo
		//	return 0;
		// });
		template<typename F>
		inline int ForeachFrame(F&& YuvaHandler) {
			vpx_codec_ctx_t ctx;
			vpx_codec_dec_cfg_t cfg{ 1, this->width, this->height };
			assert(this->codecId); //auto&& iface = this->codecId ? vpx_codec_vp9_dx() : vpx_codec_vp8_dx();
			auto&& iface = vpx_codec_vp9_dx();
			if (int r = vpx_codec_dec_init(&ctx, iface, &cfg, 0)) return __LINE__;	// VPX_CODEC_OK == 0
			auto sgCtx = MakeScopeGuard([&] {
				vpx_codec_destroy(&ctx);
				});

			uint8_t const* rgbBuf = nullptr, * aBuf = nullptr;
			uint32_t rgbBufLen = 0, aBufLen = 0;

			if (this->hasAlpha) {
				vpx_codec_ctx_t ctxAlpha;
				if (int r = vpx_codec_dec_init(&ctxAlpha, iface, &cfg, 0)) return __LINE__;
				auto sgCtxAlpha = MakeScopeGuard([&] {
					vpx_codec_destroy(&ctxAlpha);
					});

				for (uint32_t i = 0; i < this->count; ++i) {
					if (int r = this->GetFrameBuf(i, rgbBuf, rgbBufLen, aBuf, aBufLen)) return r;

					if (int r = vpx_codec_decode(&ctx, rgbBuf, rgbBufLen, nullptr, 0)) return __LINE__;
					if (int r = vpx_codec_decode(&ctxAlpha, aBuf, aBufLen, nullptr, 0)) return __LINE__;

					vpx_codec_iter_t iterator = nullptr;
					auto&& imgRGB = vpx_codec_get_frame(&ctx, &iterator);
					if (!imgRGB || imgRGB->fmt != VPX_IMG_FMT_I420) return __LINE__;
					if (imgRGB->stride[1] != imgRGB->stride[2]) return __LINE__;

					iterator = nullptr;
					auto&& imgA = vpx_codec_get_frame(&ctxAlpha, &iterator);
					if (!imgA || imgA->fmt != VPX_IMG_FMT_I420) return __LINE__;
					if (imgA->stride[0] != imgRGB->stride[0]) return __LINE__;

					if (int r = YuvaHandler(i, this->width, this->height
						, imgRGB->planes[0], imgRGB->planes[1], imgRGB->planes[2], imgA->planes[0]
						, imgRGB->stride[0], imgRGB->stride[1])) return r;
				}
			} else {
				for (uint32_t i = 0; i < this->count; ++i) {
					if (int r = this->GetFrameBuf(i, rgbBuf, rgbBufLen)) return r;

					if (int r = vpx_codec_decode(&ctx, rgbBuf, rgbBufLen, nullptr, 0)) return __LINE__;

					vpx_codec_iter_t iterator = nullptr;
					auto&& imgRGB = vpx_codec_get_frame(&ctx, &iterator);
					if (!imgRGB || imgRGB->fmt != VPX_IMG_FMT_I420) return __LINE__;
					if (imgRGB->stride[1] != imgRGB->stride[2]) return __LINE__;

					if (int r = YuvaHandler(i, this->width, this->height
						, imgRGB->planes[0], imgRGB->planes[1], imgRGB->planes[2], nullptr	// no alpha
						, imgRGB->stride[0], imgRGB->stride[1])) return r;
				}
			}

			return 0;
		}

	};
}

namespace XxmvTest {

	struct Scene : SceneBase {
		void Init(GameLooper* looper) override;
		int Update() override;

		xx::Xxmv mv;
	};
}
