#include "xx2d.h"

#ifdef XX2D_ENABLE_MINIAUDIO
#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"
#undef L
#undef C
#undef R
#endif

namespace xx {

	Audio::Audio() {
		cd = 0.1;
#ifdef XX2D_ENABLE_MINIAUDIO
		ctx = malloc(sizeof(ma_engine));
		if (auto result = ma_engine_init(nullptr, (ma_engine*)ctx); result != MA_SUCCESS) {
			throw std::logic_error("Failed to initialize audio engine.");
		}
#endif
	}

	Audio::~Audio() {
#ifdef XX2D_ENABLE_MINIAUDIO
		tp.Join();
		if (bg) {
			ma_sound_uninit((ma_sound*)bg);
			free(bg);
			bg = {};
		}
		ma_engine_uninit((ma_engine*)ctx);
		free(ctx);
		ctx = {};
#endif
	}

	void Audio::StopBG() {
#ifdef XX2D_ENABLE_MINIAUDIO
		tp.Add([this] {
			if (bg) {
				ma_sound_uninit((ma_sound*)bg);
				free(bg);
				bg = {};
			}
		});
#endif
	}

	void Audio::PlayBG(std::string_view const& fn) {
		auto s = xx::engine.GetFullPath(fn);
#ifdef XX2D_ENABLE_MINIAUDIO
		tp.Add([this, s = std::move(s)] {
			if (bg) {
				ma_sound_uninit((ma_sound*)bg);
			} else {
				bg = malloc(sizeof(ma_sound));
			}
			auto result = ma_sound_init_from_file((ma_engine*)ctx, s.c_str(), 0, nullptr, nullptr, (ma_sound*)bg);
			if (result != MA_SUCCESS) {
				throw std::logic_error(xx::ToString("Failed to init sound file. result = ", result, " fn = ", s));
			}
			ma_sound_start((ma_sound*)bg);
		});
#endif
	}

	void Audio::Play(std::string_view const& fn) {
		auto s = xx::engine.GetFullPath(fn);
#ifdef XX2D_ENABLE_MINIAUDIO
		tp.Add([this, s = std::move(s)] {
			for (int i = (int)fncds.size() - 1; i >= 0; --i) {
				auto& pair = fncds[i];
				if (pair.second <= xx::engine.nowSecs) {
					pair = fncds.back();
					fncds.pop_back();
				} else if (pair.first == s) return;
			}
			ma_engine_play_sound((ma_engine*)ctx, s.c_str(), nullptr);
			fncds.emplace_back(std::move(s), xx::engine.nowSecs + cd);
		});
#endif
	}

}
