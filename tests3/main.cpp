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
	xx_assert(frames_plane_blue.size() == 5);

	tp.GetToByPrefix(frames_plane_red, "plane_r");
	xx_assert(frames_plane_red.size() == 5);

	tp.GetToByPrefix(frames_bomb, "bomb");
	xx_assert(frames_bomb.size() == 1);

	tasks.Add(MasterLogic());
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
	// sleep a while
	//co_await gEngine.TaskSleep(5);

	// create player's plane
	//for (size_t i = 0; i < 10000; i++)
	{
		player_planes.emplace_back(xx::Make<Plane>())->Init(0);
		player_planes.emplace_back(xx::Make<Plane>())->Init(1);
		co_yield 0;
	}
	
	// todo: create monsters?
	while (true) {
		co_yield 0;
	}
}

void Plane::Init(int planeIndex_) {
	// data init
	planeIndex = planeIndex_;
	frames = planeIndex == 0 ? &gLooper->frames_plane_blue : &gLooper->frames_plane_red;
	speed0 = gPlaneNormalSpeed;
	speed1 = speed0 * gScale;
	frameIndexs[1] = 0;
	frameIndexs[2] = frames->size() / 2;
	frameIndexs[3] = frames->size() - 1;
	frameIndexs[0] = frameIndexs[2];
	frameChangeSpeed = speed0 * 0.2;
	radius = 9 * gScale;
	godMode = true;
	visible = false;

	// bomb data init
	for (size_t i = 0; i < 30; i++) {
		auto&& b = bombs.Emplace();
		b.type = BombTypes::Trident;
	}

	// brush partial init
	texBrush.SetScale(gScale);

	// script init
	tasks.Add(Born());
	tasks.Add(Shine());
	tasks.Add(SyncBombPos());
}

void Plane::InitBombPos() {
	if (auto n = bombs.Count()) {
		float y = pos.y - gBombAnchorYDist;
		for (size_t i = 0; i < n; i++) {
			bombs[i].pos = { pos.x, y - gBombDiameter * i };
		}
	}
}

xx::Task<> Plane::SyncBombPos() {
	while (true) {
		co_yield 0;
		auto n = bombs.Count();
		if (!n) continue;
		
		auto tarPos = xx::XY{ pos.x, pos.y - gBombAnchorYDist };
		for (size_t i = 0; i < n; i++) {
			auto& pos = bombs[i].pos;

			auto d = tarPos - pos;
			auto dd = d.x * d.x + d.y * d.y;
			if (gBombMinSpeedPow2 > dd) {
				pos = tarPos;
			}
			else {
				auto inc = d.As<float>() / (i ? gBombFollowSteps : gBombFirstFollowSteps);
				pos += inc;
			}
			tarPos = { pos.x, pos.y - (moving ? gBombMinSpeed : gBombDiameter) };
		}
	}
}

xx::Task<> Plane::Born() {
	pos = { gPlaneBornXs[planeIndex], gPlaneBornYFrom };
	InitBombPos();
	moving = true;
	while (gPlaneBornYTo - pos.y > speed1) {
		pos.y += gPlaneBornSpeed * gScale;
		co_yield 0;
	}
	pos.y = gPlaneBornYTo;
	
	tasks.Add(Update());			// switch to normal mode
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
		// bak for change frame display
		auto oldPos = pos;

		// move by mouse ois
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
		if (oldPos.x < pos.x) {
			fidx += frameChangeSpeed;
			if (fidx > fmax) {
				fidx = fmax;
			}
		}
		else if (oldPos.x > pos.x) {
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

		// sync state
		moving = oldPos != pos;
		co_yield 0;
	}
}

void Plane::Draw() {
	// draw bombs
	for (auto i = (ptrdiff_t)bombs.Count() - 1; i >= 0; --i) {
		auto& b = bombs[i];
		texBrush.SetPosition(b.pos)
			.SetFrame(gLooper->frames_bomb[(int)b.type])
			.Draw();
	}

	if (visible) {
		// draw plane body
		texBrush.SetPosition(pos)
			.SetFrame(frames->operator[](frameIndexs[0]))
			.Draw();
	}
}
