#include "main.h"

int main() {
	auto g = std::make_unique<GameLooper>();
	gLooper = g.get();
	gEngine.w = gDesignWidth * gDisplayScale;
	gEngine.h = gDesignHeight * gDisplayScale;
	return g->Run("arcade: gemini wings");
}

void GameLooper::AfterGLInit() {
	fontBase = gEngine.LoadBMFont("res/font/coderscrux.fnt"sv);

	xx::TP tp;
	tp.Load("res/gemini/gemini.plist");

	tp.GetToByPrefix(frames_plane_blue, "plane_blue");
	tp.GetToByPrefix(frames_plane_red, "plane_red");
	tp.GetToByPrefix(frames_bullet_plane, "plane_bullet");
	tp.GetToByPrefix(frames_bomb, "bomb");
	tp.GetToByPrefix(frames_monster_strawberry, "monster_strawberry");
	tp.GetToByPrefix(frames_monster_dragonfly, "monster_dragonfly");
	tp.GetToByPrefix(frames_explosion_monster, "explosion_monster");
	tp.GetToByPrefix(frames_explosion_bigmonster, "explosion_bigmonster");

	tasks.Add(MasterLogic());
}

int GameLooper::Update() {
	timePool += gEngine.delta;
	while (timePool >= gFds) {
		timePool -= gFds;
		nowSecs += gFds;
		tasks();

		for (auto& o : player_planes) {
			if (o) {
				o->tasks();
			}
		}

		monster_strawberries.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		monster_dragonflies.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		explosion_monsters.Foreach([&](auto& o) {
			return o->Update.Resume();
		});

		// todo: more Update
	}

	xx::Quad texBrush;
	texBrush.SetScale(gDisplayScale);

	explosion_monsters.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	monster_strawberries.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	monster_dragonflies.Foreach([&](auto& o) {
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
	//co_await gEngine.TaskSleep(5);

	// create player's plane
	//for (size_t i = 0; i < 10000; i++)
	{
		player_planes.emplace_back(xx::Make<Plane>())->Init(0);
		//player_planes.emplace_back(xx::Make<Plane>())->Init(1);
		co_yield 0;
	}
	
	xx::MovePathCache dragonflyPath1, dragonflyPath2;
	{
		xx::MovePath mp;
		xx::CurvePoints cps;
		cps.points.emplace_back().pos = { g9Pos7X + 190, g9Pos7Y - 10 };
		cps.points.emplace_back().pos = { g9Pos7X + 30, g9Pos7Y - 100 };
		cps.points.emplace_back().pos = { g9Pos7X + 194, g9Pos7Y - 174 };
		cps.points.emplace_back().pos = { g9Pos7X + 30, g9Pos7Y - 250 };
		mp.Clear();
		mp.FillCurve(cps.isLoop, cps.points);
		dragonflyPath1.Init(mp, 1);

		cps.points.clear();
		cps.points.emplace_back().pos = { g9Pos9X - 190, g9Pos9Y - 10 };
		cps.points.emplace_back().pos = { g9Pos9X - 30, g9Pos9Y - 100 };
		cps.points.emplace_back().pos = { g9Pos9X - 194, g9Pos9Y - 174 };
		cps.points.emplace_back().pos = { g9Pos9X - 30, g9Pos9Y - 250 };
		mp.Clear();
		mp.FillCurve(cps.isLoop, cps.points);
		dragonflyPath2.Init(mp, 1);
	}

	while (nowSecs < 10) {
		co_await gEngine.TaskSleep(1.f / 10);

		// todo: if (plane is left  use path 1  else use path 2)
		monster_dragonflies.Emplace().Emplace()->Init( gRnd.Next<bool>() ? &dragonflyPath1 : &dragonflyPath2);
	}

	// todo: create more monsters?

#if 0
	while (true) {
		co_yield 0;
		for (int i = 0; i < 100; ++i) {
			monster_strawberries.Emplace().Emplace()->Init();
		}
	}
#else
	while (true) {
		co_await gEngine.TaskSleep(1.f / 10);

		monster_strawberries.Emplace().Emplace()->Init();
	}
#endif
}

void Plane::Init(int planeIndex_) {
	// data init
	planeIndex = planeIndex_;
	speed = gPlaneNormalSpeed;
	frameIndex = gPlaneFrameIndexMid;
	frameChangeSpeed = speed * 0.2;
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
			if (b.pos.y - gPlaneBulletHight_2 > gDesignHeight_2) return true;	// flying out of the screen 
			{
				auto [idx, next] = gLooper->monster_strawberries.FindIf([&](xx::Shared<MonsterStrawberry>& o)->bool {
					auto d = b.pos - o->pos;
					auto constexpr rr = (gPlaneBulletHight_2 + gMonsterStrawberryRadius)
						* (gPlaneBulletHight_2 + gMonsterStrawberryRadius);
					auto dd = d.x * d.x + d.y * d.y;
					if (dd < rr) {
						// +score ??
						gLooper->explosion_monsters.Emplace().Emplace()->Init(o->pos);
						return true;
					}
					return false;
					});
				if (idx != -1) {
					gLooper->monster_strawberries.Remove(idx, next);
					return true;
				}
			}
			{
				auto [idx, next] = gLooper->monster_dragonflies.FindIf([&](xx::Shared<MonsterDragonfly>& o)->bool {
					auto d = b.pos - o->pos;
					auto constexpr rr = (gPlaneBulletHight_2 + gMonsterDragonflyRadius)
						* (gPlaneBulletHight_2 + gMonsterDragonflyRadius);
					auto dd = d.x * d.x + d.y * d.y;
					if (dd < rr) {
						// +score ??
						gLooper->explosion_monsters.Emplace().Emplace()->Init(o->pos, true);
						return true;
					}
					return false;
					});
				if (idx != -1) {
					gLooper->monster_dragonflies.Remove(idx, next);
					return true;
				}
			}

			// todo: find other monsters ?
			// todo: optimize performance ( space index ? )
			return false;
		});
	}
}

xx::Task<> Plane::Born() {
	pos = { gPlaneBornXs[planeIndex], gPlaneBornYFrom };
	InitBombPos();
	moving = true;
	while (gPlaneBornYTo - pos.y > gPlaneBornSpeed) {
		pos.y += gPlaneBornSpeed;
		co_yield 0;
	}
	pos.y = gPlaneBornYTo;
	
	tasks.Add(Update());			// switch to normal mode
}

xx::Task<> Plane::Shine() {
	auto e = gLooper->nowSecs + 5;
	do {
		visible += gPlaneVisibleInc;
		co_yield 0;
		co_yield 0;
	} while (gLooper->nowSecs < e);

	visible = 0;
	godMode = false;			// close god mode
}

xx::Task<> Plane::Update() {
	while (true) {
		// bak for change frame display
		auto oldPos = pos;

		// fire
		if (gEngine.Pressed(xx::Mbtns::Left) && gLooper->nowSecs >= bulletNextFireTime) {
			bulletNextFireTime = gLooper->nowSecs + gPlaneBulletFireCD;			// apply cd effect

			bullets.Emplace(xx::XY{ pos.x, pos.y + gPlaneBulletFireYOffset });
		}

		// use bomb
		if (gEngine.Pressed(xx::Mbtns::Right) && gLooper->nowSecs >= bulletNextFireTime) {
			bombNextUseTime = gLooper->nowSecs + gPlaneBulletFireCD;				// apply cd effect

			// todo: switch( first bomb type ) ....
		}

		// move by mouse ois
		auto mp = gMousePos * g1_DisplayScale;
		auto d = mp - pos;
		auto dd = d.x * d.x + d.y * d.y;

		if (speed * speed > dd) {
			pos = mp;
		}
		else {
			auto inc = d.As<float>() / std::sqrt(float(dd)) * speed;
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
		texBrush.SetFrame(gLooper->frames_bomb[(int)b.type]).SetPosition(b.pos * gDisplayScale).Draw();
	}

	// draw plane
	if ((int)visible << 31 == 0) {
		auto& frames = (planeIndex == 0 ? gLooper->frames_plane_blue : gLooper->frames_plane_red);
		texBrush.SetFrame(frames[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
	}

	// draw bullets
	texBrush.SetFrame(gLooper->frames_bullet_plane[(int)bulletBeginFrameIndex]);
	bullets.Foreach([&](PlaneBullet& b) {
		texBrush.SetPosition(xx::XY{ b.pos.x - gPlaneBulletSpacing_2, b.pos.y } * gDisplayScale).Draw();
		texBrush.SetPosition(xx::XY{ b.pos.x + gPlaneBulletSpacing_2, b.pos.y } * gDisplayScale).Draw();
	});
}



void ExplosionMonster::Init(xx::XY const& pos_, bool isBig) {
	pos = pos_;
	if (isBig) {
		frameIndex = gExplosionBigMonsterFrameIndexMin;
		Update = UpdateBig_();
		frames = &gLooper->frames_explosion_bigmonster;
	}
	else {
		frameIndex = gExplosionMonsterFrameIndexMin;
		Update = Update_();
		frames = &gLooper->frames_explosion_monster;
	}
}
xx::Task<> ExplosionMonster::Update_() {
	while (true) {
		frameIndex += gExplosionMonsterFrameSwitchDelay;
		if ((int)frameIndex > gExplosionMonsterFrameIndexMax) break;
		co_yield 0;
	}
};
xx::Task<> ExplosionMonster::UpdateBig_() {
	while (true) {
		frameIndex += gExplosionMonsterFrameSwitchDelay;
		if ((int)frameIndex > gExplosionBigMonsterFrameIndexMax) break;
		co_yield 0;
	}
};

void ExplosionMonster::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(frames->operator[](frameIndex)).SetPosition(pos * gDisplayScale).Draw();
}



void MonsterStrawberry::Init() {
	if (gRnd.Next<bool>()) {
		pos.x = -gMonsterStrawberryRadius - gDesignWidth_2;
		inc = { gMonsterStrawberryHorizontalMoveSpeed, 0 };
	}
	else {
		pos.x = gMonsterStrawberryRadius + gDesignWidth_2;
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
	while (pos.y > -gDesignHeight_2 - gMonsterStrawberryDiameter) {
		co_yield 0;
		pos += inc;
		frameIndex += gMonsterStrawberryVerticalMoveFrameSwitchDelay;
		if ((int)frameIndex > gMonsterStrawberryVerticalFrameIndexMax) break;
	}

	// vertical move repeat
	frameIndex = gMonsterStrawberryVerticalRepeatFrameIndexMin + (frameIndex - gMonsterStrawberryVerticalFrameIndexMax - 1);
	while (pos.y > -gDesignHeight_2 - gMonsterStrawberryDiameter) {
		co_yield 0;
		pos += inc;
		frameIndex += gMonsterStrawberryHorizontalMoveFrameSwitchDelay;
		if ((int)frameIndex > gMonsterStrawberryVerticalRepeatFrameIndexMax) {
			frameIndex = gMonsterStrawberryVerticalRepeatFrameIndexMin + (frameIndex - gMonsterStrawberryVerticalRepeatFrameIndexMax - 1);
		}
	}
};

void MonsterStrawberry::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_strawberry[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}



void MonsterDragonfly::Init(xx::MovePathCache* path_) {
	path = path_;
	frameIndex = gRnd.Next((float)gMonsterDragonflyFrameIndexMin, gMonsterDragonflyFrameIndexMax + 0.999f);
}

xx::Task<> MonsterDragonfly::Update_() {
	do {
		totalDistance += gMonsterDragonflySpeed;
		auto&& o = path->Move(totalDistance);
		if (!o) break;
		pos = o->pos;
		frameIndex += gMonsterDragonflyFrameSwitchDelay;
		if ((int)frameIndex > gMonsterDragonflyFrameIndexMax) {
			frameIndex = gMonsterDragonflyFrameIndexMin + (frameIndex - gMonsterDragonflyFrameIndexMax - 1);
		}
		co_yield 0;
	} while (pos.y > -gDesignHeight_2 - gMonsterDragonflyDiameter);
};

void MonsterDragonfly::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_dragonfly[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}
