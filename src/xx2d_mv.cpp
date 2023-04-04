#include "xx2d.h"
#include <vpx_decoder.h>
#include <vp8dx.h>
#include <ebml_parser.h>

namespace xx {

	int Mv::LoadFromXxmv(xx::Data_r d) {
		Clear();
		// todo: verify header? version number?
		if (d.len < 8 || memcmp(d.buf, "XXMV 1.0", 8)) return __LINE__;
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
		return FillBufs();
	}

	int Mv::FillBufs() {
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

	int Mv::SaveToXxmv(std::filesystem::path const& path) {
		xx::Data d;
		d.WriteBuf("XXMV 1.0", 8);
		d.WriteFixed(codecId);
		d.WriteFixed(hasAlpha);
		d.WriteFixed(width);
		d.WriteFixed(height);
		d.WriteFixed(duration);
		d.WriteFixed((uint32_t)lens.size());
		d.WriteBuf(lens.data(), lens.size() * sizeof(uint32_t));
		d.WriteFixed((uint32_t)data.len);
		d.WriteBuf(data.buf, data.len);
		return xx::WriteAllBytes(path, d);
	}

	int Mv::LoadFromWebm(xx::Data_r d) {
		Clear();

		auto data = std::make_unique<uint8_t[]>(d.len);
		memcpy(data.get(), d.buf, d.len);

		// begin parse ebml header
		auto&& ebml = parse_ebml_file(std::move(data), d.len/*, 1*/);
		auto&& segment = ebml.FindChildById(EbmlElementId::Segment);

		// get total play duration
		auto&& info = segment->FindChildById(EbmlElementId::Info);
		auto&& duration = info->FindChildById(EbmlElementId::Duration);
		this->duration = (float)std::stod(duration->value());

		// vp8/9
		auto&& tracks = segment->FindChildById(EbmlElementId::Tracks);
		auto&& trackEntry = tracks->FindChildById(EbmlElementId::TrackEntry);
		auto&& codecId = trackEntry->FindChildById(EbmlElementId::CodecID);
		this->codecId = codecId->value() == "V_VP8" ? 0 : 1;

		// w,h
		auto&& video = trackEntry->FindChildById(EbmlElementId::Video);
		auto&& pixelWidth = video->FindChildById(EbmlElementId::PixelWidth);
		this->width = std::stoi(pixelWidth->value());
		auto&& pixelHeight = video->FindChildById(EbmlElementId::PixelHeight);
		this->height = std::stoi(pixelHeight->value());

		// alpha?
		auto&& _alphaMode = video->FindChildById(EbmlElementId::AlphaMode);
		this->hasAlpha = _alphaMode->value() == "1" ? 1 : 0;

		std::vector<int> frames;
		uint32_t frameNumber = 0;

		std::list<EbmlElement>::const_iterator clusterOwner;
		if (this->codecId == 0) {
			clusterOwner = segment;
		} else {
			auto&& tags = segment->FindChildById(EbmlElementId::Tags);
			auto&& tag = tags->FindChildById(EbmlElementId::Tag);
			clusterOwner = tag->FindChildById(EbmlElementId::Targets);
		}

		auto&& cluster = clusterOwner->FindChildById(EbmlElementId::Cluster);
		while (cluster != clusterOwner->children().cend()) {
			auto timecode = cluster->FindChildById(EbmlElementId::Timecode);
			auto clusterPts = std::stoi(timecode->value());

			if (this->hasAlpha) {
				auto&& blockGroup = cluster->FindChildById(EbmlElementId::BlockGroup);
				while (blockGroup != cluster->children().cend()) {
					{
						// get yuv data + size
						auto&& block = blockGroup->FindChildById(EbmlElementId::Block);
						auto&& data = block->data();
						auto&& size = block->size();

						// fix yuv data + size
						size_t track_number_size_length;
						(void)get_ebml_element_size(data, size, track_number_size_length);
						data = data + track_number_size_length + 3;
						size = size - track_number_size_length - 3;

						this->lens.push_back((uint32_t)size);
						this->data.WriteBuf(data, size);
					}
					{
						// get a data + size
						auto&& blockAdditions = blockGroup->FindChildById(EbmlElementId::BlockAdditions);
						auto&& blockMore = blockAdditions->FindChildById(EbmlElementId::BlockMore);
						auto&& blockAdditional = blockMore->FindChildById(EbmlElementId::BlockAdditional);
						auto&& data_alpha = blockAdditional->data();
						auto&& size_alpha = (uint32_t)blockAdditional->size();

						this->lens.push_back((uint32_t)size_alpha);
						this->data.WriteBuf(data_alpha, size_alpha);
					}

					// next
					blockGroup = cluster->FindNextChildById(++blockGroup, EbmlElementId::BlockGroup);
					++frameNumber;
				}
			} else {
				auto&& simpleBlock = cluster->FindChildById(EbmlElementId::SimpleBlock);
				while (simpleBlock != cluster->children().cend()) {
					auto&& data = simpleBlock->data();
					auto&& size = simpleBlock->size();

					// fix yuv data + size
					size_t track_number_size_length;
					(void)get_ebml_element_size(data, size, track_number_size_length);
					data = data + track_number_size_length + 3;
					size = size - track_number_size_length - 3;

					this->lens.push_back((uint32_t)size);
					this->data.WriteBuf(data, size);

					// next
					simpleBlock = cluster->FindNextChildById(++simpleBlock, EbmlElementId::BlockGroup);
					++frameNumber;
				}
			}

			cluster = clusterOwner->FindNextChildById(++cluster, EbmlElementId::Cluster);
		}

		return FillBufs();
	}

	int Mv::Load(xx::Data_r d) {
		if (d.len < 8) return __LINE__;
		if (memcmp(d.buf, "\x1a\x45\xdf\xa3", 4) == 0) return LoadFromWebm(d);
		else if (memcmp(d.buf, "XXMV", 4) == 0) return LoadFromXxmv(d);
		else return __LINE__;
	}


	Mv::operator bool() const {
		return count != 0;
	}

	void Mv::Clear() {
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

	int Mv::GetFrameBuf(uint32_t const& idx, uint8_t const*& rgbBuf, uint32_t& rgbBufLen) const {
		if (hasAlpha) return __LINE__;
		if (idx >= count) return __LINE__;
		rgbBuf = bufs[idx];
		rgbBufLen = lens[idx];
		return 0;
	}

	int Mv::GetFrameBuf(uint32_t idx, uint8_t const*& rgbBuf, uint32_t& rgbBufLen, uint8_t const*& aBuf, uint32_t& aBufLen) const {
		if (!hasAlpha) return __LINE__;
		if (idx >= count) return __LINE__;
		idx *= 2;
		rgbBuf = bufs[idx];
		aBuf = bufs[idx + 1];
		rgbBufLen = lens[idx];
		aBufLen = lens[idx + 1];
		return 0;
	}

	int Mv::ForeachFrame(YuvaHandler const& h) const {

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

				if (int r = h(i, this->width, this->height, imgRGB->planes[0], imgRGB->planes[1], imgRGB->planes[2], imgA->planes[0], imgRGB->stride[0], imgRGB->stride[1])) return r;
			}
		} else {
			for (uint32_t i = 0; i < this->count; ++i) {
				if (int r = this->GetFrameBuf(i, rgbBuf, rgbBufLen)) return r;

				if (int r = vpx_codec_decode(&ctx, rgbBuf, rgbBufLen, nullptr, 0)) return __LINE__;

				vpx_codec_iter_t iterator = nullptr;
				auto&& imgRGB = vpx_codec_get_frame(&ctx, &iterator);
				if (!imgRGB || imgRGB->fmt != VPX_IMG_FMT_I420) return __LINE__;
				if (imgRGB->stride[1] != imgRGB->stride[2]) return __LINE__;

				if (int r = h(i, this->width, this->height, imgRGB->planes[0], imgRGB->planes[1], imgRGB->planes[2], nullptr, imgRGB->stride[0], imgRGB->stride[1])) return r;
			}
		}

		return 0;
	}

}

