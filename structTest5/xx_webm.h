#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

/*
usage example:

	xx::Webm wm;
	if (int r = wm.LoadFromWebm("res/a.webm")) return r;		// load a.webm file, parse ebml info, store data
	if (int r = wm.SaveToXxmv("res/a.xxmv")) return r;			// write xxmv header + data to file

	if (int r = wm.LoadFromXxmv("res/a.xxmv")) return r;		// load a.xxmv file, save info & data
	if (int r = wm.SaveToPngs("res/", "a")) return r;			// write every frame to a0.png, a1.png, a2.png .... files

	if (int r = wm.SaveToPackedPngs("res/", "a")) return r;		// texture packer likely. write all frame to a?.png + a?.plist

	// todo: runtime export to cocos plist dict + texture

pics -> webm command line:
	./ffmpeg -f image2 -framerate 60 -i "??????(%d).png" -c:v libvpx-vp9 -pix_fmt yuva420p -b:v 1000K -speed 0 ??????.webm
*/

#include "ebml_parser.h"
#include "xx_data.h"
#include "xx_file.h"

#define SVPNG_LINKAGE inline
#define SVPNG_OUTPUT xx::Data& d
#define SVPNG_PUT(u) d.WriteFixed((uint8_t)(u));
#include "svpng.inc"

#include "libyuv.h"
#include "vpx_decoder.h"
#include "vp8dx.h"

namespace xx {
	// webm 解析后的容易使用的存储形态
	struct Webm {
		// 0: vp8;	1: vp9
		uint8_t codecId = 0;

		// 1: true
		uint8_t hasAlpha = 0;

		// 像素宽
		uint16_t width = 0;

		// 像素高
		uint16_t height = 0;

		// 总播放时长( 秒 )
		float duration = 0;

		// 所有帧数据长度集合( hasAlpha 则为 rgb / a 交替长度集合 )
		std::vector<uint32_t> lens;

		// 所有帧数据依次排列
		xx::Data data;

		// 所有帧 rgb / a 数据块指针( 后期填充 )
		std::vector<uint8_t*> bufs;

		// 总帧数( 后期填充 )
		uint32_t count = 0;

		// todo: 可能还有别的附加信息存储. 例如 中心点坐标, 显示缩放比
		// todo: 动画信息? 多少帧到多少帧 定义为一个动画? 每帧的显示延迟设定? 时间轴?

		Webm() = default;
		Webm(Webm const&) = delete;
		Webm(Webm&&) = default;
		Webm& operator=(Webm const&) = delete;
		Webm& operator=(Webm&&) = default;

		inline operator bool() {
			return count != 0;
		}

		// 填充基础数据或反序列化后继续 填充 bufs, count
		inline int Init() {
			count = (uint32_t)(hasAlpha ? lens.size() / 2 : lens.size());

			bufs.resize(lens.size());
			auto baseBuf = data.buf;
			for (int i = 0; i < lens.size(); ++i) {
				bufs[i] = (uint8_t*)baseBuf;
				baseBuf += lens[i];
			}
			if (baseBuf != data.buf + data.len) return -1;
			return 0;
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
			if (hasAlpha) return -1;
			if (idx >= count) return -2;
			rgbBuf = bufs[idx];
			rgbBufLen = lens[idx];
			return 0;
		}

		inline int GetFrameBuf(uint32_t idx, uint8_t const*& rgbBuf, uint32_t& rgbBufLen, uint8_t const*& aBuf, uint32_t& aBufLen) {
			if (!hasAlpha) return -1;
			if (idx >= count) return -2;
			idx *= 2;
			rgbBuf = bufs[idx];
			aBuf = bufs[idx + 1];
			rgbBufLen = lens[idx];
			aBufLen = lens[idx + 1];
			return 0;
		}


		// 从 .xxmv 读出数据并填充到 wm. 成功返回 0
		inline int LoadFromXxmv(std::filesystem::path const& path) {
			this->Clear();

			xx::Data d;
			if (int r = xx::ReadAllBytes(path, d)) return r;
			if (int r = d.ReadFixed(codecId)) return r;
			if (int r = d.ReadFixed(hasAlpha)) return r;
			if (int r = d.ReadFixed(width)) return r;
			if (int r = d.ReadFixed(height)) return r;
			if (int r = d.ReadFixed(duration)) return r;
			uint32_t siz;
			if (int r = d.ReadFixed(siz)) return r;
			lens.resize(siz);
			if (int r = d.ReadBuf(lens.data(), lens.size() * sizeof(uint32_t))) return r;
			if (int r = d.ReadFixed(siz)) return r;
			data.Resize(siz);
			if (int r = d.ReadBuf(data.buf, data.len)) return r;
			return Init();
		}

		inline int SaveToXxmv(std::filesystem::path const& path) {
			xx::Data d;
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

		// 从 .webm 读出数据并填充到 wm. 成功返回 0
		inline int LoadFromWebm(std::filesystem::path const& path) {
			this->Clear();

			// 读文件
			std::unique_ptr<uint8_t[]> data;
			size_t dataLen;
			if (int r = xx::ReadAllBytes(path, data, dataLen)) return r;

			// 开始解析 ebml 头
			auto&& ebml = parse_ebml_file(std::move(data), dataLen/*, 1*/);
			auto&& segment = ebml.FindChildById(EbmlElementId::Segment);

			// 提取 播放总时长
			auto&& info = segment->FindChildById(EbmlElementId::Info);
			auto&& duration = info->FindChildById(EbmlElementId::Duration);
			this->duration = (float)std::stod(duration->value());

			// 提取 编码方式
			auto&& tracks = segment->FindChildById(EbmlElementId::Tracks);
			auto&& trackEntry = tracks->FindChildById(EbmlElementId::TrackEntry);
			auto&& codecId = trackEntry->FindChildById(EbmlElementId::CodecID);
			this->codecId = codecId->value() == "V_VP8" ? 0 : 1;

			// 提取 宽高
			auto&& video = trackEntry->FindChildById(EbmlElementId::Video);
			auto&& pixelWidth = video->FindChildById(EbmlElementId::PixelWidth);
			this->width = std::stoi(pixelWidth->value());
			auto&& pixelHeight = video->FindChildById(EbmlElementId::PixelHeight);
			this->height = std::stoi(pixelHeight->value());

			// 判断 是否带 alpha 通道
			auto&& _alphaMode = video->FindChildById(EbmlElementId::AlphaMode);
			this->hasAlpha = _alphaMode->value() == "1" ? 1 : 0;

			std::vector<int> frames;
			uint32_t frameNumber = 0;

			std::list<EbmlElement>::const_iterator clusterOwner;
			if (this->codecId == 0) {
				clusterOwner = segment;
			}
			else {
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
				}
				else {
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

			if (int r = this->Init()) return r;
			return 0;
		}

		// 每帧画面另存为 png 文件。
		inline int SaveToPngs(std::filesystem::path const& path, std::string const& prefix) {
			uint32_t i = 0;
			return ForeachFrame([&](std::vector<uint8_t> const& bytes)->int {
				auto&& fn = path / (prefix + std::to_string(i) + ".png");
				if (int r = RgbaSaveToPng(fn, bytes.data(), this->width, this->height)) return r;
				++i;
				return 0;
				});
		}

		// f = [](std::vector<uint8_t> const& bytes)->int { ... }
		template<typename F>
		inline int ForeachFrame(F&& f) {
			vpx_codec_ctx_t ctx;
			vpx_codec_dec_cfg_t cfg{ 1, this->width, this->height };
			auto&& iface = this->codecId ? vpx_codec_vp9_dx() : vpx_codec_vp8_dx();
			if (int r = vpx_codec_dec_init(&ctx, iface, &cfg, 0)) return r;	// because VPX_CODEC_OK == 0

			uint8_t const* rgbBuf = nullptr, * aBuf = nullptr;
			uint32_t rgbBufLen = 0, aBufLen = 0;
			std::vector<uint8_t> bytes;

			if (this->hasAlpha) {
				vpx_codec_ctx_t ctxAlpha;
				if (int r = vpx_codec_dec_init(&ctxAlpha, iface, &cfg, 0)) return r;

				for (uint32_t i = 0; i < this->count; ++i) {
					if (int r = this->GetFrameBuf(i, rgbBuf, rgbBufLen, aBuf, aBufLen)) return r;

					if (int r = vpx_codec_decode(&ctx, rgbBuf, rgbBufLen, nullptr, 0)) return r;
					if (int r = vpx_codec_decode(&ctxAlpha, aBuf, aBufLen, nullptr, 0)) return r;

					vpx_codec_iter_t iterator = nullptr;
					auto&& imgRGB = vpx_codec_get_frame(&ctx, &iterator);
					if (!imgRGB || imgRGB->fmt != VPX_IMG_FMT_I420) return -1;
					if (imgRGB->stride[1] != imgRGB->stride[2]) return -2;

					iterator = nullptr;
					auto&& imgA = vpx_codec_get_frame(&ctxAlpha, &iterator);
					if (!imgA || imgA->fmt != VPX_IMG_FMT_I420) return -3;
					if (imgA->stride[0] != imgRGB->stride[0]) return -4;

					if (int r = Yuva2Rgba(bytes, this->width, this->height
						, imgRGB->planes[0], imgRGB->planes[1], imgRGB->planes[2], imgA->planes[0]
						, imgRGB->stride[0], imgRGB->stride[1])) return r;
					if (int r = f(bytes)) return r;
				}
			}
			else {
				for (uint32_t i = 0; i < this->count; ++i) {
					if (int r = this->GetFrameBuf(i, rgbBuf, rgbBufLen)) return r;

					if (int r = vpx_codec_decode(&ctx, rgbBuf, rgbBufLen, nullptr, 0)) return r;

					vpx_codec_iter_t iterator = nullptr;
					auto&& imgRGB = vpx_codec_get_frame(&ctx, &iterator);
					if (!imgRGB || imgRGB->fmt != VPX_IMG_FMT_I420) return -1;
					if (imgRGB->stride[1] != imgRGB->stride[2]) return -2;

					if (int r = Yuva2Rgba(bytes, this->width, this->height
						, imgRGB->planes[0], imgRGB->planes[1], imgRGB->planes[2], nullptr
						, imgRGB->stride[0], imgRGB->stride[1])) return r;
					if (int r = f(bytes)) return r;
				}
			}

			return 0;
		}

		inline static int Yuva2Rgba(std::vector<uint8_t>& bytes
			, uint32_t const& w, uint32_t const& h
			, uint8_t const* const& yData, uint8_t const* const& uData, uint8_t const* const& vData, uint8_t const* const& aData
			, uint32_t const& yaStride, uint32_t const& uvStride) {

#ifdef LIBYUV_API
			// 这个 windows x64 下大约快 20+ 倍
			bytes.resize(w * h * 4);
			return libyuv::I420AlphaToARGB(yData, yaStride, uData, uvStride, vData, uvStride, aData, yaStride, bytes.data(), w * 4, w, h, 0);
#else
			// 这段代码逻辑可写入 shader
			bytes.clear();
			bytes.reserve(w * h * 4);

			// 产生像素坐标
			for (uint32_t _h = 0; _h < h; ++_h) {
				for (uint32_t _w = 0; _w < w; ++_w) {
					// 根据坐标结合具体宽高跨距算下标. uv 每个像素对应 ya 4个像素
					auto&& yaIdx = yaStride * _h + _w;
					auto&& uvIdx = uvStride * (_h / 2) + _w / 2;

					// 得到 yuv 原始数据, byte -> float
					auto&& y = yData[yaIdx] / 255.0f;
					auto&& u = uData[uvIdx] / 255.0f;
					auto&& v = vData[uvIdx] / 255.0f;

					// 进一步修正
					y = 1.1643f * (y - 0.0625f);
					u = u - 0.5f;
					v = v - 0.5f;

					// 算出 rgb( float 版 )
					auto&& r = y + 1.5958f * v;
					auto&& g = y - 0.39173f * u - 0.81290f * v;
					auto&& b = y + 2.017f * u;

					// 裁剪为 0 ~ 1
					if (r > 1.0f) r = 1.0f; else if (r < 0.0f) r = 0.0f;
					if (g > 1.0f) g = 1.0f; else if (g < 0.0f) g = 0.0f;
					if (b > 1.0f) b = 1.0f; else if (b < 0.0f) b = 0.0f;

					// 存起来
					bytes.push_back((uint8_t)(b * 255));
					bytes.push_back((uint8_t)(g * 255));
					bytes.push_back((uint8_t)(r * 255));
					bytes.push_back(aData ? aData[yaIdx] : (uint8_t)0);
				}
			}
			return 0;
#endif
		}

		inline static int RgbaSaveToPng(std::filesystem::path const& fn, uint8_t const* const& bytes, uint32_t const& w, uint32_t const& h) {
			xx::Data d;
			svpng(d, w, h, bytes, 1);
			return xx::WriteAllBytes(fn, d);
		}

		// 将 srcPos(小图) 绘制 到 dstPos(大图) 的 x,y (bmp左上角). 因为是图片打包用途，故必须确保 space 装得下, 没有边缘问题
		// 先计算第一行第一个像素 要从 space 的哪个下标 开始填, 每次填 bw 长, 填完后 下标 += sw 填 bh 次
		inline static void Draw(uint32_t* dstPos, uint32_t const& sw, uint32_t const& sh
			, uint32_t* srcPos, uint32_t const& bw, uint32_t const& bh, uint32_t const& x, uint32_t const& y) {
			assert(x >= 0 && y >= 0 && x + bw < sw&& y + bh < sh);
			dstPos += sw * y + x;
			auto dstPosEnd = dstPos + sw * bh;
			auto copyLen = bw * sizeof(uint32_t);
			for (; dstPos < dstPosEnd; dstPos += sw) {
				memcpy(dstPos, srcPos, copyLen);
				srcPos += bw;
			}
		}

		struct PListMaker {
			std::string data;

			PListMaker& Begin() {
				data = R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
    <dict>
        <key>frames</key>
        <dict>)";
				return *this;
			}

			PListMaker& Append(std::string_view const& name
				, int const& ssW, int const& ssH		/* spriteSize */
				, int const& trX, int const& trY		/* textureRect */
				/*, bool const& textureRotated*/
			) {
				auto wh = std::to_string(ssW) + "," + std::to_string(ssH);
				data += std::string(R"(
            <key>)") + std::string(name) + R"(</key>
            <dict>
                <key>aliases</key>
                <array/>
                <key>spriteOffset</key>
                <string>{0,0}</string>
                <key>spriteSize</key>
                <string>{)" + wh + R"(}</string>
                <key>spriteSourceSize</key>
                <string>{)" + wh + R"(}</string>
                <key>textureRect</key>
				<string>{{)" + std::to_string(trX) + "," + std::to_string(trY) + "},{" + wh + R"(}}</string>
                <key>textureRotated</key>
                <false/>
            </dict>)";
				return *this;
			}

			PListMaker& End(std::string_view const& picFileName
				, int const& sW, int const& sH		/* size */
			) {
				data += std::string(R"(
        </dict>
        <key>metadata</key>
        <dict>
            <key>format</key>
            <integer>3</integer>
            <key>pixelFormat</key>
            <string>RGBA8888</string>
            <key>premultiplyAlpha</key>
            <false/>
            <key>realTextureFileName</key>
            <string>)") + std::string(picFileName) + R"(</string>
            <key>size</key>
            <string>{)" + std::to_string(sW) + "," + std::to_string(sH) + R"(}</string>
            <key>smartupdate</key>
            <string></string>
            <key>textureFileName</key>
            <string>)" + std::string(picFileName) + R"(</string>
        </dict>
    </dict>
</plist>
)";
				return *this;
			}

			PListMaker& SaveToFile(std::filesystem::path const& fn) {
				xx::WriteAllBytes(fn, data.data(), data.size());
				return *this;
			}
		};

		// 将所有帧的图打包存为 N 张 png 大图( 类似 texture packer ). 文件名为 path / prefix + 0,1,2,... + .png
		// todo: 像素级查找 包围盒 并得到实际尺寸? 然后弄个 矩形填充算法？
		inline int SaveToPackedPngs(std::filesystem::path const& path, std::string const& prefix, std::string itemPrefix = ""/* prefix + " (" */, std::string const& itemSuffix = ").png", int itemBeginNum = 1, uint32_t const& sw = 4096, uint32_t const& sh = 4096) {
			if (itemPrefix.empty()) {
				itemPrefix = prefix + " (";
			}
			PListMaker pm;
			pm.Begin();
			std::vector<uint32_t> space;
			space.resize(sw * sh);
			int numRows = sh / height;
			int numCols = sw / width;
			int x = 0, y = 0, z = 0;
			int r = ForeachFrame([&](std::vector<uint8_t> const& bytes)->int {
				Draw(space.data(), sw, sh, (uint32_t*)bytes.data(), width, height, x * width, y * height);
				pm.Append(itemPrefix + std::to_string(itemBeginNum) + itemSuffix, width, height, x * width, y * height);
				++itemBeginNum;
				++x;
				if (x == numCols) {
					x = 0;
					++y;
				}
				if (y == numRows) {
					y = 0;
					pm.End(prefix + std::to_string(z), sw, sh).SaveToFile(path / (prefix + std::to_string(z) + ".plist")).Begin();
					RgbaSaveToPng(path / (prefix + std::to_string(z) + ".png"), (uint8_t*)space.data(), sw, sh);
					++z;
					memset(space.data(), 0, space.size() * sizeof(uint32_t));
				}
				return 0;
				});
			if (r) return r;
			if (x || y) {
				pm.End(prefix + std::to_string(z) + ".png", sw, sh).SaveToFile(path / (prefix + std::to_string(z) + ".plist"));
				return RgbaSaveToPng(path / (prefix + std::to_string(z) + ".png"), (uint8_t*)space.data(), sw, (y + 1) * height);
			}

			return 0;
		}
	};
}


//
//std::string FloatToString(float const& v) {
//	std::string s(15, '\0');
//	s.resize(sprintf(s.data(), "%g", v));
//	return s;
//}
//PListMaker& Append(std::string_view const& name
//	, float const& soX, float const& soY	/* spriteOffset */
//	, int const& ssW, int const& ssH		/* spriteSize */
//	, int const& sssW, int const& sssH		/* spriteSourceSize */
//	, int const& trX, int const& trY, int const& trW, int const& trH		/* textureRect */
//	/*, bool const& textureRotated*/
//) {
//	data += std::string(R"(
//            <key>)") + std::string(name) + R"(</key>
//            <dict>
//                <key>aliases</key>
//                <array/>
//                <key>spriteOffset</key>
//                <string>{)" + FloatToString(soX) + "," + FloatToString(soY) + R"(}</string>
//                <key>spriteSize</key>
//                <string>{)" + std::to_string(ssW) + "," + std::to_string(ssH) + R"(}</string>
//                <key>spriteSourceSize</key>
//                <string>{)" + std::to_string(sssW) + "," + std::to_string(sssH) + R"(}</string>
//                <key>textureRect</key>
//				<string>{{)" + std::to_string(trX) + "," + std::to_string(trY) + "},{" + std::to_string(trW) + "," + std::to_string(trH) + R"(}}</string>
//                <key>textureRotated</key>
//                <false/>
//            </dict>)";
//	return *this;
//}
