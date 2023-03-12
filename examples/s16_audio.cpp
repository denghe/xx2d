#include "xx2d_pch.h"
#include "game_looper.h"
#include "s16_audio.h"

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

namespace AudioTest {

	Audio::Audio() {
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

	void Audio::Play(std::string_view fn) {
		auto s = xx::engine.GetFullPath(fn);
#ifdef XX2D_ENABLE_MINIAUDIO
		ma_engine_play_sound((ma_engine*)ctx, s.c_str(), nullptr);
#endif
	}

	void Audio::PlayBG(std::string_view fn) {
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


	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "AudioTest::Scene::Init" << std::endl;

		meListener.Init(xx::Mbtns::Left); 

		float x = 0, xstep = xx::engine.w / 3 - 50;
		float y = 300, yinc = 100;

		btns.emplace_back().Init(this, { -xstep, y }, 0, "play bg.ogg", 32);
		btns.emplace_back().Init(this, { 0, y }, -1, "stop bg.ogg", 32);

		y -= yinc;
		btns.emplace_back().Init(this, { -xstep, y }, 1, "play 1.ogg", 32);
		btns.emplace_back().Init(this, { 0, y }, 2, "play 2.ogg", 32);
		btns.emplace_back().Init(this, { xstep, y }, 3, "play 3.ogg", 32);

		y -= yinc;
		btns.emplace_back().Init(this, { -xstep, y }, 4, "play 4.ogg", 32);
		btns.emplace_back().Init(this, { 0, y }, 5, "play 5.ogg", 32);
		btns.emplace_back().Init(this, { xstep, y }, 6, "play 6.ogg", 32);

	}

	int Scene::Update() {

		// handle mouse event
		meListener.Update();
		auto&& iter = btns.begin();
		while (meListener.eventId && iter != btns.end()) {
			meListener.Dispatch(&*iter++);
		}

		// layered draw for auto batch
		for (auto&& b : btns) {
			b.content.Draw();
		}
		for (auto&& b : btns) {
			b.border.Draw();
		}

		return 0;
	}

	bool Button::HandleMouseDown(ButtonMouseEventListener& L) {
		return Inside(L.downPos);
	}

	int Button::HandleMouseMove(ButtonMouseEventListener& L) {
		return 0;
	}

	void Button::HandleMouseUp(ButtonMouseEventListener& L) {
		if (Inside(xx::engine.mousePosition)) {
			switch (id) {
			case -1:
				scene->audio.StopBG();
				break;
			case 0:
				scene->audio.PlayBG("res/bg.ogg"sv);
				break;
			case 1:
				scene->audio.Play("res/1.ogg"sv);
				break;
			case 2:
				scene->audio.Play("res/2.ogg"sv);
				break;
			case 3:
				scene->audio.Play("res/3.ogg"sv);
				break;
			case 4:
				scene->audio.Play("res/4.ogg"sv);
				break;
			case 5:
				scene->audio.Play("res/5.ogg"sv);
				break;
			case 6:
				scene->audio.Play("res/6.ogg"sv);
				break;
			}
		}
	}

	void Button::Init(Scene* const& scene_, xx::XY const& pos, int const& id_, std::string_view const& txt_, float const& fontSize) {
		scene = scene_;
		txt = txt_;
		id = id_;
		content.SetText(scene->looper->fontBase, txt, fontSize)
			.SetPosition(pos).Commit();
		auto siz = content.size + xx::XY{ 20, 4 };
		border.FillBoxPoints({}, siz)
			.SetPosition(pos);
		siz /= 2;
		leftBottom = pos - siz;
		rightTop = pos + siz;
	}

	bool Button::Inside(xx::XY const& p) {
		return p.x >= leftBottom.x && p.x <= rightTop.x
			&& p.y >= leftBottom.y && p.y <= rightTop.y;
	}

}
