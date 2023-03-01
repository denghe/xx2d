#include "xx2d_pch.h"
#include "game_looper.h"
#include "s16_audio.h"

#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"
#undef L
#undef C
#undef R

namespace AudioTest {

	Audio::Audio() {
		ctx = malloc(sizeof(ma_engine));
		if (auto result = ma_engine_init(nullptr, (ma_engine*)ctx); result != MA_SUCCESS) {
			throw std::logic_error("Failed to initialize audio engine.");
		}
	}

	Audio::~Audio() {
		ma_engine_uninit((ma_engine*)ctx);
		free(ctx);
	}

	void Audio::Play(std::string_view fn) {
		auto s = xx::engine.GetFullPath(fn);
		ma_engine_play_sound((ma_engine*)ctx, s.c_str(), nullptr);
	}


	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "AudioTest::Scene::Init" << std::endl;

		meListener.Init(xx::Mbtns::Left);

		auto&& b = btns.emplace_back();
		b.Init(this, {}, "play", 32);
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
			scene->audio.Play("res/1.ogg"sv);
		}
	}

	void Button::Init(Scene* const& scene, xx::XY const& pos, std::string_view const& txt_, float const& fontSize) {
		this->scene = scene;
		txt = txt_;
		content.SetText(scene->looper->fnt1, txt, fontSize)
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
