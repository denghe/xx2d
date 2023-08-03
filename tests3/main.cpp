#include "main.h"

int main() {
	auto g = std::make_unique<GameLooper>();
	gLooper = g.get();
	gEngine.w = gWndWidth;
	gEngine.h = gWndHeight;
	return g->Run("arcade: gemini wings");
}

void GameLooper::AfterGLInit() {
	fontBase = gEngine.LoadBMFont("res/font/coderscrux.fnt"sv);
	xx::TP tp;
	tp.Load("res/gemini/gemini.plist");
	tp.GetToByPrefix(frames_plane_blue, "plane_b");
	tp.GetToByPrefix(frames_plane_red, "plane_r");

	tasks.AddTask(MasterLogic());
}

int GameLooper::Update() {
	timePool += gEngine.delta;
	while (timePool >= gFds) {
		timePool -= gFds;
		tasks();

		for (auto& o : player_planes) {
			if (o) {
				o->tasks();
			}
		}

		// todo: more Update
	}

	for (auto& o : player_planes) {
		if (o) {
			o->Draw();
		}
	}

	// todo: more Draw

	fpsViewer.Draw(fontBase);
	return 0;
}

xx::Task<> GameLooper::MasterLogic() {
	// sleep 2s
	co_await gEngine.TaskSleep(2);

	// create player's plane
	player_planes[0].Emplace<Plane>()->Init();
	
	// todo: create monsters?
	while (true) {
		co_yield 0;
	}
}

void Plane::Init(int planeIndex_) {
	// data init
	planeIndex = planeIndex_;
	speed0 = gPlaneNormalSpeed;
	speed1 = speed0 * gScale;
	frameIndexs[1] = 0;
	frameIndexs[2] = gLooper->frames_plane_blue.size() / 2;
	frameIndexs[3] = gLooper->frames_plane_blue.size() - 1;
	frameIndexs[0] = frameIndexs[2];
	frameChangeSpeed = speed0 * 0.2;
	radius = 9 * gScale;

	// brush partial init
	body.SetScale(gScale);

	// script init
	tasks.AddTask(Born());
	tasks.AddTask(Shine());
}

xx::Task<> Plane::Born() {
	pos = { gPlaneBornXs[planeIndex], gPlaneBornYFrom };
	while (gPlaneBornYTo - pos.y > speed1) {
		pos.y += gPlaneBornSpeed * gScale;
		co_yield 0;
	}
	pos.y = gPlaneBornYTo;
	
	tasks.AddTask(Update());			// switch to normal mode
}

xx::Task<> Plane::Shine() {
	auto e = gEngine.nowSecs + 5;
	do {
		visible = !visible;
		co_yield 0;
		co_yield 0;
	} while (gEngine.nowSecs < e);

	visible = true;
	godMode = false;			// close god mode
}

xx::Task<> Plane::Update() {
	while (true) {
		auto x = pos.x;	// bak for change frame compare

		auto d = gMousePos - pos;
		auto dd = d.x * d.x + d.y * d.y;

		if (speed1 * speed1 > dd) {
			pos = gMousePos;
		}
		else {
			auto inc = d.As<float>() / std::sqrt(float(dd)) * speed1;
			pos += inc;
		}

		// change frame display
		float &fidx = frameIndexs[0], &fmin = frameIndexs[1], &fmid = frameIndexs[2], &fmax = frameIndexs[3];
		if (x < pos.x) {
			fidx += frameChangeSpeed;
			if (fidx > fmax) {
				fidx = fmax;
			}
		}
		else if (x > pos.x) {
			fidx -= frameChangeSpeed;
			if (fidx < fmin) {
				fidx = fmin;
			}
		}
		else {
			if (fidx > fmid) {
				fidx -= frameChangeSpeed;
				if (fidx < fmid) {
					fidx = fmid;
				}
			}
			else if (fidx < fmid) {
				fidx += frameChangeSpeed;
				if (fidx > fmid) {
					fidx = fmid;
				}
			}
		}

		co_yield 0;
	}
}

void Plane::Draw() {
	if (!visible) return;
	body.SetPosition(pos)
		.SetFrame(gLooper->frames_plane_blue[frameIndexs[0]])
		.Draw();
}
