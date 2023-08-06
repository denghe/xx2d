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

	tp.GetToByPrefix(frames_plane_blue, "plane_blue");
	tp.GetToByPrefix(frames_plane_red, "plane_red");
	tp.GetToByPrefix(frames_bullet_plane, "plane_bullet");
	frames_bullet_plane.push_back(frames_bullet_plane.back());	// plane bullet last frame repeat 1 time
	tp.GetToByPrefix(frames_bomb, "bomb");
	tp.GetToByPrefix(frames_monster_strawberry, "monster_strawberry");

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

		monster_strawberries.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		// todo: more Update
	}

	xx::Quad texBrush;
	texBrush.SetScale(gDisplayScale);

	monster_strawberries.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	for (auto& o : player_planes) {
		if (o) {
			o->Draw(texBrush);
		}
	}

	// todo: more Draw

	fpsViewer.Draw(fontBase);
	return 0;
}

xx::Task<> GameLooper::MasterLogic() {
	// sleep a while for OBS record
	co_await gEngine.TaskSleep(5);

	// create player's plane
	//for (size_t i = 0; i < 10000; i++)
	{
		player_planes.emplace_back(xx::Make<Plane>())->Init(0);
		//player_planes.emplace_back(xx::Make<Plane>())->Init(1);
		co_yield 0;
	}
	
	// todo: create monsters?

	//monster_strawberries.Emplace().Emplace()->Init();

	//while (true) {
	//	co_yield 0;
	//	for (int i = 0; i < 100; ++i) {
	//		monster_strawberries.Emplace().Emplace()->Init();
	//	}
	//}

	while (true) {
		co_await gEngine.TaskSleep(1.f / 10);

		monster_strawberries.Emplace().Emplace()->Init();
	}
}

void Plane::Init(int planeIndex_) {
	// data init
	planeIndex = planeIndex_;
	speed0 = gPlaneNormalSpeed;
	speed1 = speed0 * gDisplayScale;
	frameIndex = gPlaneFrameIndexMid;
	frameChangeSpeed = speed0 * 0.2;
	radius = 9 * gDisplayScale;
	godMode = true;
	visible = 0;

	// bombs init
	for (size_t i = 0; i < 15; i++) {
		auto&& b = bombs.Emplace();
		b.type = BombTypes(gRnd.Next(5));
	}

	// script init
	tasks.Add(Born());
	tasks.Add(Shine());
	tasks.Add(SyncBombPos());
	tasks.Add(SyncBulletPosCol());
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
				pos += d.As<float>() / (i ? (moving ? gBombMovingFollowSteps : gBombStopFollowSteps) : gBombFirstFollowSteps);
			}
			tarPos = { pos.x, pos.y - (moving ? 0 : gBombDiameter) };
		}
	}
}

xx::Task<> Plane::SyncBulletPosCol() {
	while (true) {
		co_yield 0;

		// loop switch tex
		bulletBeginFrameIndex += gPlaneBulletFrameChangeStep;
		if ((size_t)bulletBeginFrameIndex >= gLooper->frames_bullet_plane.size()) {
			bulletBeginFrameIndex = 0;
		}

		bullets.Foreach([](PlaneBullet& b)->bool {
			b.pos.y += gPlaneBulletSpeed;
			if (b.pos.y - gPlaneBulletHight_2 > gWndHeight_2) return true;	// flying out of the screen 
			// todo: optimize performance ( space index ? )
			auto [idx, next] = gLooper->monster_strawberries.FindIf([&](xx::Shared<MonsterStrawberry>& o)->bool {
				auto d = b.pos - o->pos;
				auto constexpr rr = (gPlaneBulletHight_2 + gMonsterStrawberryRadius)
					* (gPlaneBulletHight_2 + gMonsterStrawberryRadius);
				auto dd = d.x * d.x + d.y * d.y;
				return dd < rr;
			});
			if (idx != -1) {
				// todo: effect
				gLooper->monster_strawberries.Remove(idx, next);
				return true;
			}
			return false;
		});
	}
}

xx::Task<> Plane::Born() {
	pos = { gPlaneBornXs[planeIndex], gPlaneBornYFrom };
	InitBombPos();
	moving = true;
	while (gPlaneBornYTo - pos.y > speed1) {
		pos.y += gPlaneBornSpeed * gDisplayScale;
		co_yield 0;
	}
	pos.y = gPlaneBornYTo;
	
	tasks.Add(Update());			// switch to normal mode
}

xx::Task<> Plane::Shine() {
	auto e = gEngine.nowSecs + 5;
	do {
		visible += gPlaneVisibleInc;
		co_yield 0;
		co_yield 0;
	} while (gEngine.nowSecs < e);

	visible = 0;
	godMode = false;			// close god mode
}

xx::Task<> Plane::Update() {
	while (true) {
		// bak for change frame display
		auto oldPos = pos;


		// fire
		if (gEngine.Pressed(xx::Mbtns::Left) && gEngine.nowSecs >= bulletNextFireTime) {
			bulletNextFireTime = gEngine.nowSecs + gPlaneBulletFireCD;			// apply cd effect

			bullets.Emplace(xx::XY{ pos.x, pos.y + gPlaneBulletFireYOffset });
		}

		// use bomb
		if (gEngine.Pressed(xx::Mbtns::Right) && gEngine.nowSecs >= bulletNextFireTime) {
			bombNextUseTime = gEngine.nowSecs + gPlaneBulletFireCD;				// apply cd effect

			// todo: switch( first bomb type ) ....
		}

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
		if (oldPos.x < pos.x) {
			frameIndex += frameChangeSpeed;
			if (frameIndex > gPlaneFrameIndexMax) {
				frameIndex = gPlaneFrameIndexMax;
			}
		}
		else if (oldPos.x > pos.x) {
			frameIndex -= frameChangeSpeed;
			if (frameIndex < gPlaneFrameIndexMin) {
				frameIndex = gPlaneFrameIndexMin;
			}
		}
		else {
			if (frameIndex > gPlaneFrameIndexMid) {
				frameIndex -= frameChangeSpeed;
				if (frameIndex < gPlaneFrameIndexMid) {
					frameIndex = gPlaneFrameIndexMid;
				}
			}
			else if (frameIndex < gPlaneFrameIndexMid) {
				frameIndex += frameChangeSpeed;
				if (frameIndex > gPlaneFrameIndexMid) {
					frameIndex = gPlaneFrameIndexMid;
				}
			}
		}

		// sync state
		moving = oldPos != pos;
		co_yield 0;
	}
}

void Plane::Draw(xx::Quad& texBrush) {
	// draw bombs
	for (auto i = (ptrdiff_t)bombs.Count() - 1; i >= 0; --i) {
		auto& b = bombs[i];
		texBrush.SetFrame(gLooper->frames_bomb[(int)b.type]).SetPosition(b.pos).Draw();
	}

	// draw plane
	if ((int)visible << 31 == 0) {
		auto& frames = (planeIndex == 0 ? gLooper->frames_plane_blue : gLooper->frames_plane_red);
		texBrush.SetFrame(frames[frameIndex]).SetPosition(pos).Draw();
	}

	// draw bullets
	texBrush.SetFrame(gLooper->frames_bullet_plane[(int)bulletBeginFrameIndex]);
	bullets.Foreach([&](PlaneBullet& b) {
		texBrush.SetPosition({ b.pos.x - gPlaneBulletSpacing_2, b.pos.y }).Draw();
		texBrush.SetPosition({ b.pos.x + gPlaneBulletSpacing_2, b.pos.y }).Draw();
	});
}



void MonsterStrawberry::Init() {
	if (gRnd.Next<bool>()) {
		pos.x = -gMonsterStrawberryRadius - gWndWidth_2;
		inc = { gMonsterStrawberryHorizontalMoveSpeed, 0 };
	}
	else {
		pos.x = gMonsterStrawberryRadius + gWndWidth_2;
		inc = { -gMonsterStrawberryHorizontalMoveSpeed, 0 };
	}
	pos.y = gRnd.Next(gMonsterStrawberryBornYFrom, gMonsterStrawberryBornYTo);
	frameIndex = gRnd.Next((float)gMonsterStrawberryHorizontalFrameIndexMin, gMonsterStrawberryHorizontalFrameIndexMax + 0.999f);
	switchDelay = gRnd.Next(gMonsterStrawberrySwitchToVerticalMoveDelayFrom, gMonsterStrawberrySwitchToVerticalMoveDelayTo);
}

xx::Task<> MonsterStrawberry::Update_() {
	// horizontal move
	while (true) {
		co_yield 0;
		pos += inc;
		if (--switchDelay <= 0) break;
		frameIndex += gMonsterStrawberryHorizontalMoveFrameSwitchDelay;
		if ((int)frameIndex > gMonsterStrawberryHorizontalFrameIndexMax) {
			frameIndex = gMonsterStrawberryHorizontalFrameIndexMin + (frameIndex - gMonsterStrawberryHorizontalFrameIndexMax - 1);
		}
	}

	// vertical move begin
	inc = { 0, -gMonsterStrawberryVerticalMoveSpeed };
	frameIndex = gMonsterStrawberryVerticalFrameIndexMin;
	while (pos.y > -gWndHeight_2 - gMonsterStrawberryDiameter) {
		co_yield 0;
		pos += inc;
		frameIndex += gMonsterStrawberryVerticalMoveFrameSwitchDelay;
		if ((int)frameIndex > gMonsterStrawberryVerticalFrameIndexMax) break;
	}

	// vertical move repeat
	frameIndex = gMonsterStrawberryVerticalRepeatFrameIndexMin + (frameIndex - gMonsterStrawberryVerticalFrameIndexMax - 1);
	while (pos.y > -gWndHeight_2 - gMonsterStrawberryDiameter) {
		co_yield 0;
		pos += inc;
		frameIndex += gMonsterStrawberryHorizontalMoveFrameSwitchDelay;
		if ((int)frameIndex > gMonsterStrawberryVerticalRepeatFrameIndexMax) {
			frameIndex = gMonsterStrawberryVerticalRepeatFrameIndexMin + (frameIndex - gMonsterStrawberryVerticalRepeatFrameIndexMax - 1);
		}
	}
};

void MonsterStrawberry::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_strawberry[frameIndex]).SetPosition(pos).Draw();
}
