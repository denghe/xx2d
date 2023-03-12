#include "xx2d_pch.h"
#include "xx2d_audio.h"

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
		cd = 0.05;
#ifdef XX2D_ENABLE_MINIAUDIO
		ctx = malloc(sizeof(ma_engine));
		if (auto result = ma_engine_init(nullptr, (ma_engine*)ctx); result != MA_SUCCESS) {
			throw std::logic_error("Failed to initialize audio engine.");
		}
#endif
	}

	Audio::~Audio() {
#ifdef XX2D_ENABLE_MINIAUDIO
		StopBG();
		ma_engine_uninit((ma_engine*)ctx);
		free(ctx);
		ctx = {};
#endif
	}

	void Audio::StopBG() {
#ifdef XX2D_ENABLE_MINIAUDIO
		if (bg) {
			ma_sound_uninit((ma_sound*)bg);
			free(bg);
			bg = {};
		}
#endif
	}

	void Audio::Play(std::string_view const& fn) {
		auto s = xx::engine.GetFullPath(fn);
		if (!CanPlay(s)) return;
#ifdef XX2D_ENABLE_MINIAUDIO
		ma_engine_play_sound((ma_engine*)ctx, s.c_str(), nullptr);
#endif
	}

	void Audio::PlayBG(std::string_view const& fn) {
		auto s = xx::engine.GetFullPath(fn);
#ifdef XX2D_ENABLE_MINIAUDIO
		StopBG();
		bg = malloc(sizeof(ma_sound));
		auto result = ma_sound_init_from_file((ma_engine*)ctx, s.c_str(), 0, nullptr, nullptr, (ma_sound*)bg);
		if (result != MA_SUCCESS) {
			throw std::logic_error(xx::ToString("Failed to init sound file. result = ", result, " fn = ", fn));
		}
		ma_sound_start((ma_sound*)bg);
#endif
	}

	bool Audio::CanPlay(std::string_view const& fn) {
		for (auto& pair : fncds) {
			if (pair.first == fn) {
				if (pair.second > xx::engine.nowSecs) return false;
				else {
					pair.second = xx::engine.nowSecs + cd;
					return true;
				}
			}
		}
		fncds.emplace_back(fn, xx::engine.nowSecs + cd);
		return true;
	}

}
