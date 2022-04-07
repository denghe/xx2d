/*
功能：
	针对 cocos, 提供加载 .xxmv 文件到 sprite frame cache 的能力

代码示例:
	xx::XxmvCocos mv;	// 可公用
	...

	int r = mv.LoadFromXxmv("zhangyu.xxmv");
	assert(!r);
	r = mv.FillToSpriteFrameCache("a", "", "");
	assert(!r);

	cocos2d::Vector<cocos2d::SpriteFrame*> sfs;
	auto sfc = cocos2d::SpriteFrameCache::getInstance();
	for (int i = 1; i <= mv.count; ++i) {
		auto sf = sfc->getSpriteFrameByName(std::string("a") + std::to_string(i));
		assert(sf);
		sfs.pushBack(sf);
	}
	auto animation = cocos2d::Animation::createWithSpriteFrames(sfs, 1.f / 60);
	sprite->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(animation)));

*/

// todo: 跳过 plist 构造 & 解析? 直接生成最终 sprite frame ?

// todo: 为进一步提升加载性能，以及增加易用性，线程池并行加载？


#include "cocos2d.h"
#include "vp8dx.h"
#include "vpx_decoder.h"
#include "libyuv.h"

#include "xx_data.h"

#define SVPNG_LINKAGE inline
#define SVPNG_OUTPUT xx::Data& d
#define SVPNG_PUT(u) d.WriteFixed((uint8_t)(u));
#include "svpng.inc"

namespace xx {
	// xxmv 解析后的容易使用的存储形态
	struct XxmvCocos {

		// 文件数据容器
		cocos2d::Data fileData;

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


		// 指向 依次排列的所有帧数据 的数据区( 后期填充 )
		uint8_t* data = nullptr;

		// 所有帧 rgb / a 数据块指针( 后期填充 )
		std::vector<uint8_t*> bufs;

		// 总帧数( 后期填充 )
		uint32_t count = 0;

		// todo: 可能还有别的附加信息存储. 例如 中心点坐标, 显示缩放比
		// todo: 动画信息? 多少帧到多少帧 定义为一个动画? 每帧的显示延迟设定? 时间轴?

		XxmvCocos() = default;
		XxmvCocos(XxmvCocos const&) = delete;
		XxmvCocos(XxmvCocos&&) = default;
		XxmvCocos& operator=(XxmvCocos const&) = delete;
		XxmvCocos& operator=(XxmvCocos&&) = default;

		inline operator bool() {
			return count != 0;
		}

	protected:
		inline void Clear() {
			fileData.clear();
			codecId = 0;
			hasAlpha = 0;
			width = 0;
			height = 0;
			duration = 0;
			lens.clear();
			data = nullptr;
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

	public:
		// 从 .xxmv 读出数据. 成功返回 0 (	不兼容大尾机器 )
		inline int LoadFromXxmv(std::string_view const& path) {
			this->Clear();
			auto fs = cocos2d::FileUtils::getInstance();
			fileData = fs->getDataFromFile(path);
			if (fileData.isNull()) return __LINE__;

			auto p = fileData.getBytes();
			auto len = fileData.getSize();
			auto pe = p + len;

			// todo: verify version number & data?
			if (p + 14 > pe) return __LINE__;
			codecId = *p;
			p += 1;

			hasAlpha = *p;
			p += 1;

			width = *p + (p[1] << 8);
			p += 2;

			height = *p + (p[1] << 8);
			p += 2;

			memcpy(&duration, p, 4);
			p += 4;

			uint32_t siz;
			memcpy(&siz, p, 4);
			count = (uint32_t)(hasAlpha ? siz / 2 : siz);
			p += 4;

			if (p + siz * 4 > pe) return __LINE__;
			lens.resize(siz);
			memcpy(lens.data(), p, siz * 4);
			p += siz * 4;

			if (p + 4 > pe) return __LINE__;
			memcpy(&siz, p, 4);
			p += 4;

			if (p + siz > pe) return __LINE__;
			data = p;

			bufs.resize(lens.size());
			for (int i = 0; i < lens.size(); ++i) {
				bufs[i] = p;
				p += lens[i];
			}
			if (p != pe) return __LINE__;
			return 0;
		}

		template<class F>
		inline static auto MakeScopeGuard(F&& f) noexcept {
			struct ScopeGuard {
				F f;
				bool cancel;

				explicit ScopeGuard(F&& f) noexcept : f(std::move(f)), cancel(false) {}

				~ScopeGuard() noexcept { if (!cancel) { f(); } }

				inline void Cancel() noexcept { cancel = true; }

				inline void operator()(bool cancel = false) {
					f();
					this->cancel = cancel;
				}
			};
			return ScopeGuard(std::forward<F>(f));
		}


		// f = [](std::vector<uint8_t> const& bytes)->int { ... }
		template<typename F>
		inline int ForeachFrame(F&& f) {
			vpx_codec_ctx_t ctx;
			vpx_codec_dec_cfg_t cfg{ 1, this->width, this->height };
			if (this->codecId == 0) return __LINE__;	// does not support vp8 now
			auto&& iface = vpx_codec_vp9_dx(); //auto&& iface = this->codecId ? vpx_codec_vp9_dx() : vpx_codec_vp8_dx();
			if (int r = vpx_codec_dec_init(&ctx, iface, &cfg, 0)) return __LINE__;	// VPX_CODEC_OK == 0
			auto sgCtx = MakeScopeGuard([&] {
				vpx_codec_destroy(&ctx);
				});

			uint8_t const* rgbBuf = nullptr, * aBuf = nullptr;
			uint32_t rgbBufLen = 0, aBufLen = 0;
			std::vector<uint8_t> bytes;

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

					if (int r = Yuva2Rgba(bytes, this->width, this->height
						, imgRGB->planes[0], imgRGB->planes[1], imgRGB->planes[2], imgA->planes[0]
						, imgRGB->stride[0], imgRGB->stride[1])) return __LINE__;
					if (int r = f(bytes)) return r;
				}
			}
			else {
				for (uint32_t i = 0; i < this->count; ++i) {
					if (int r = this->GetFrameBuf(i, rgbBuf, rgbBufLen)) return r;

					if (int r = vpx_codec_decode(&ctx, rgbBuf, rgbBufLen, nullptr, 0)) return __LINE__;

					vpx_codec_iter_t iterator = nullptr;
					auto&& imgRGB = vpx_codec_get_frame(&ctx, &iterator);
					if (!imgRGB || imgRGB->fmt != VPX_IMG_FMT_I420) return __LINE__;
					if (imgRGB->stride[1] != imgRGB->stride[2]) return __LINE__;

					if (int r = Yuva2Rgba(bytes, this->width, this->height
						, imgRGB->planes[0], imgRGB->planes[1], imgRGB->planes[2], nullptr
						, imgRGB->stride[0], imgRGB->stride[1])) return __LINE__;
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
			bytes.resize(w * h * 4);
			return libyuv::I420AlphaToABGR(yData, yaStride, uData, uvStride, vData, uvStride, aData, yaStride, bytes.data(), w * 4, w, h, 0);
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
					bytes.push_back((uint8_t)(r * 255));
					bytes.push_back((uint8_t)(g * 255));
					bytes.push_back((uint8_t)(b * 255));
					bytes.push_back(aData ? aData[yaIdx] : (uint8_t)0);
				}
			}
			return 0;
#endif
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
		};

		inline static int FillToSpriteFrameCacheCore(std::string const& plistData, uint8_t const* const& buf, int const& width, int const& height) {
			auto img = new cocos2d::Image();
			if (!img) return __LINE__;
			auto sgImg = MakeScopeGuard([&] { delete img; });

#if 1
			if (!img->initWithRawData(buf, 0, width, height, 0, false)) return __LINE__;
#else
			xx::Data png;
			png.Reserve(width * height * 4 + 1024);
			svpng(png, width, height, buf, 1);
			if (!img->initWithImageData(png.buf, png.len)) return __LINE__;
#endif

			auto t = new cocos2d::Texture2D();
			if (!t) return __LINE__;
			auto sgT = MakeScopeGuard([&] { t->release(); });
			if (!t->initWithImage(img)) return __LINE__;

			cocos2d::Data d;
			d.fastSet((uint8_t*)plistData.data(), plistData.size());
			cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFileContent(d, t);
			d.fastSet(0, 0);

			return 0;
		}

		inline int FillToSpriteFrameCache(std::string const& prefix, std::string itemPrefix = " ("/* prefix + " (" */, std::string const& itemSuffix = ").png", int itemBeginNum = 1, uint32_t const& sw = 4096, uint32_t const& sh = 4096) {
			itemPrefix = prefix + itemPrefix;
			auto sfc = cocos2d::SpriteFrameCache::getInstance();
			if (sfc->getSpriteFrameByName(itemPrefix + std::to_string(itemBeginNum) + itemSuffix)) return 0;
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
					pm.End(prefix + std::to_string(z) + ".png", sw, sh);
					if (int r = FillToSpriteFrameCacheCore(pm.data, (uint8_t*)space.data(), sw, sh)) return r;
					pm.Begin();
					++z;
					memset(space.data(), 0, space.size() * sizeof(uint32_t));
				}
				return 0;
				});
			if (r) return r;
			if (x || y) {
				pm.End(prefix + std::to_string(z) + ".png", sw, sh);
				return FillToSpriteFrameCacheCore(pm.data, (uint8_t*)space.data(), sw, sh);
			}

			return 0;
		}
	};
}
