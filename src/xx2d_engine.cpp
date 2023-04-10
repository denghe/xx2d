#include "xx2d.h"
#include "xx2d_imgui.h"

namespace xx {

	Engine engine;

	void Engine::Init() {
		nowSecs = xx::NowSteadyEpochSeconds();
		delta = 0;
		SearchPathReset();
	}

	void Engine::GLInit(void* wnd) {
		ImGuiInit(wnd);

		sm.GLInit();
		sm.Init();
	}

	void Engine::UpdateBegin() {
		assert(w >= 0 && h >= 0);
		delta = xx::NowSteadyEpochSeconds(nowSecs);

		ImGuiUpdate();

		sm.ClearCounter();
		sm.Begin();

		GLClear(RGBA8{0,0,0,0});
	}

	void Engine::GLClear(std::optional<RGBA8> const& c) {
		glViewport(0, 0, w, h);
		if (c.has_value()) {
			glClearColor(c->r / 255.f, c->g / 255.f, c->b / 255.f, c->a / 255.f);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		//glDepthMask(true);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDepthMask(false);
	}

	void Engine::GLBlendFunc(std::pair<uint32_t, uint32_t> const& bfs) {
		if (blendFuncs == bfs) return;
		blendFuncs = bfs;
		glBlendFunc(bfs.first, bfs.second);
	}

	void Engine::GLEnableBlend() {
		glEnable(GL_BLEND);
	}
	void Engine::GLDisableBlend() {
		glDisable(GL_BLEND);
	}


	void Engine::UpdateEnd() {
		sm.End();

		if (!delayFuncs.empty()) {
			for (auto& f : delayFuncs) {
				f();
			}
			delayFuncs.clear();
		}

		xx::ImGuiDraw();
	}

	void Engine::Destroy() {
		textureCache.clear();
		// ...
		xx::ImGuiDestroy();
		// ...
	}

	void Engine::SetWH(float w, float h) {
		this->w = w;
		this->h = h;
		hw = (float)w / 2;
		hh = (float)h / 2;
		ninePoints[1] = { -hw, -hh };
		ninePoints[2] = { 0, -hh };
		ninePoints[3] = { hw, -hh };
		ninePoints[4] = { -hw, 0 };
		ninePoints[5] = { 0, 0 };
		ninePoints[6] = { hw, 0 };
		ninePoints[7] = { -hw, hh };
		ninePoints[8] = { 0, hh };
		ninePoints[9] = { hw, hh };
	}
}
