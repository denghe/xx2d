#include "main.h"

int main() {
	auto g = std::make_unique<GameLooper>();
	gLooper = g.get();
	gEngine.w = gDesign.width * gDisplayScale;
	gEngine.h = gDesign.height * gDisplayScale;
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
	tp.GetToByPrefix(frames_monster_hermit_crab, "monster_hermit_crab");
	tp.GetToByPrefix(frames_monster_fly, "monster_fly");

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

		bombs.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		monsters_strawberry.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		monsters_dragonfly.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		monsters_hermit_crab.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		monsters_fly.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		explosions_monster.Foreach([&](auto& o) {
			return o->Update.Resume();
		});

		// todo: more Update
	}

	xx::Quad texBrush;
	texBrush.SetScale(gDisplayScale);

	monsters_hermit_crab.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	monsters_strawberry.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	monsters_fly.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	monsters_dragonfly.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	bombs.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	explosions_monster.Foreach([&](auto& o) {
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
	co_await gEngine.TaskSleep(3);

	// create player's plane
	//for (size_t i = 0; i < 10000; i++)
	{
		player_planes.emplace_back(xx::Make<Plane>())->Init(0);
		//player_planes.emplace_back(xx::Make<Plane>())->Init(1);
		co_yield 0;
	}

	while (nowSecs < 10) {
		co_await gEngine.TaskSleep(1.f / 5);

		monsters_hermit_crab.Emplace().Emplace()->Init();
	}
	
	xx::MovePathCache dragonflyPath1, dragonflyPath2;
	{
		xx::MovePath mp;
		xx::CurvePoints cps;
		cps.points.emplace_back().pos = { g9Pos.x7 + 150, g9Pos.y7 + gMonsterDragonfly.diameter };
		cps.points.emplace_back().pos = { g9Pos.x7 + 190, g9Pos.y7 - 10 };
		cps.points.emplace_back().pos = { g9Pos.x7 + 30, g9Pos.y7 - 100 };
		cps.points.emplace_back().pos = { g9Pos.x7 + 190, g9Pos.y7 - 170 };
		cps.points.emplace_back().pos = { g9Pos.x7 + 30, g9Pos.y7 - 250 };
		cps.points.emplace_back().pos = { g9Pos.x7 + 60, g9Pos.y3 - gMonsterDragonfly.diameter };
		mp.Clear();
		mp.FillCurve(cps.isLoop, cps.points);
		dragonflyPath1.Init(mp, 1);

		cps.points.clear();
		cps.points.emplace_back().pos = { g9Pos.x9 - 150, g9Pos.y9 + gMonsterDragonfly.diameter };
		cps.points.emplace_back().pos = { g9Pos.x9 - 190, g9Pos.y9 - 10 };
		cps.points.emplace_back().pos = { g9Pos.x9 - 30, g9Pos.y9 - 100 };
		cps.points.emplace_back().pos = { g9Pos.x9 - 190, g9Pos.y9 - 170 };
		cps.points.emplace_back().pos = { g9Pos.x9 - 30, g9Pos.y9 - 250 };
		cps.points.emplace_back().pos = { g9Pos.x9 - 60, g9Pos.y3 - gMonsterDragonfly.diameter };
		mp.Clear();
		mp.FillCurve(cps.isLoop, cps.points);
		dragonflyPath2.Init(mp, 1);
	}

	while (nowSecs < 20) {
		co_await gEngine.TaskSleep(1.f / 10);

		// todo: if (plane is left  use path 1  else use path 2)
		monsters_dragonfly.Emplace().Emplace()->Init( gRnd.Next<bool>() ? &dragonflyPath1 : &dragonflyPath2);
	}

	// todo: create more monsters?

#if 0
	while (true) {
		co_yield 0;
		for (int i = 0; i < 100; ++i) {
			monsters_strawberry.Emplace().Emplace()->Init();
		}
	}
#else
	while (true) {
		co_await gEngine.TaskSleep(1.f / 10);

		monsters_strawberry.Emplace().Emplace()->Init();
	}
#endif
}

void Plane::Init(int planeIndex_) {
	// data init
	planeIndex = planeIndex_;
	speed = gPlane.normalSpeed;
	frameIndex = gPlane.frameIndexMid;
	frameChangeSpeed = speed * 0.2;
	godMode = true;
	visible = 0;

	// bombs init
	for (size_t i = 0; i < 3; i++) {
		auto&& b = bombs.Emplace();
		b.type = BombTypes::Trident;
	}

	// script init
	tasks.Add(Born());
	tasks.Add(Shine());
	tasks.Add(SyncBombPos());
	tasks.Add(SyncBulletPosCol());
}

void Plane::InitBombPos() {
	if (auto n = bombs.Count()) {
		float y = pos.y - gBomb.anchorYDist;
		for (size_t i = 0; i < n; i++) {
			bombs[i].pos = { pos.x, y - gBomb.diameter * i };
		}
	}
}

xx::Task<> Plane::SyncBombPos() {
	while (true) {
		co_yield 0;

		// eat bomb check
		gLooper->bombs.Foreach([this](auto& o)->bool {
			auto d = pos - o->pos;
			auto constexpr rr = (gPlane.radius + gBomb.radius) * (gPlane.radius + gBomb.radius);
			auto dd = d.x * d.x + d.y * d.y;
			if (dd < rr) {
				bombs.Push(PlaneBomb{ o->type, o->pos });
				return true;
			}
			return false;
		});

		// move all tail bombs

		auto n = bombs.Count();
		if (!n) continue;
		
		auto tarPos = xx::XY{ pos.x, pos.y - gBomb.anchorYDist };
		for (size_t i = 0; i < n; i++) {
			auto& pos = bombs[i].pos;

			auto d = tarPos - pos;
			auto dd = d.x * d.x + d.y * d.y;
			if (gBomb.minSpeedPow2 > dd) {
				pos = tarPos;
			}
			else {
				pos += d.As<float>() / (i ? (moving ? gBomb.movingFollowSteps : gBomb.stopFollowSteps) : gBomb.firstFollowSteps);
			}
			tarPos = { pos.x, pos.y - (moving ? 0 : gBomb.diameter) };
		}
	}
}

xx::Task<> Plane::SyncBulletPosCol() {
	while (true) {
		co_yield 0;

		// loop switch tex
		bulletBeginFrameIndex += gPlaneBullet.frameChangeStep;
		if ((size_t)bulletBeginFrameIndex >= gLooper->frames_bullet_plane.size()) {
			bulletBeginFrameIndex = 0;
		}

		bullets.Foreach([](PlaneBullet& b)->bool {
			b.pos.y += gPlaneBullet.speed;
			if (b.pos.y - gPlaneBullet.hight_2 > gDesign.height_2) return true;	// flying out of the screen

			if (HitCheck<gMonsterStrawberry.radius, false>(b, gLooper->monsters_strawberry).has_value()) return true;
			if (HitCheck<gMonsterDragonfly.radius, true>(b, gLooper->monsters_dragonfly).has_value()) return true;
			if (HitCheck<gMonsterFly.radius, false>(b, gLooper->monsters_fly).has_value()) return true;
			if (auto pos = HitCheck<gMonsterHermitCrab.radius, false>(b, gLooper->monsters_hermit_crab); pos.has_value()) {
				gLooper->bombs.Emplace().Emplace()->Init(pos.value(), BombTypes::MAX_VALUE_UNKNOWN);
				return true;
			}

			// todo: find other monsters ?
			// todo: optimize performance ( space index ? )
			return false;
		});
	}
}

xx::Task<> Plane::Born() {
	pos = { gPlane.bornXs[planeIndex], gPlane.bornYFrom };
	InitBombPos();
	moving = true;
	while (gPlane.bornYTo - pos.y > gPlane.bornSpeed) {
		pos.y += gPlane.bornSpeed;
		co_yield 0;
	}
	pos.y = gPlane.bornYTo;
	
	tasks.Add(Update());			// switch to normal mode
}

xx::Task<> Plane::Shine() {
	auto e = gLooper->nowSecs + 5;
	do {
		visible += gPlane.visibleInc;
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
			bulletNextFireTime = gLooper->nowSecs + gPlaneBullet.fireCD;			// apply cd effect

			bullets.Emplace(xx::XY{ pos.x, pos.y + gPlaneBullet.fireYOffset });
		}

		// use bomb
		if (gEngine.Pressed(xx::Mbtns::Right) && gLooper->nowSecs >= bulletNextFireTime) {
			bombNextUseTime = gLooper->nowSecs + gPlaneBullet.fireCD;				// apply cd effect

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
			if (frameIndex > gPlane.frameIndexMax) {
				frameIndex = gPlane.frameIndexMax;
			}
		}
		else if (oldPos.x > pos.x) {
			frameIndex -= frameChangeSpeed;
			if (frameIndex < gPlane.frameIndexMin) {
				frameIndex = gPlane.frameIndexMin;
			}
		}
		else {
			if (frameIndex > gPlane.frameIndexMid) {
				frameIndex -= frameChangeSpeed;
				if (frameIndex < gPlane.frameIndexMid) {
					frameIndex = gPlane.frameIndexMid;
				}
			}
			else if (frameIndex < gPlane.frameIndexMid) {
				frameIndex += frameChangeSpeed;
				if (frameIndex > gPlane.frameIndexMid) {
					frameIndex = gPlane.frameIndexMid;
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
		texBrush.SetPosition(xx::XY{ b.pos.x - gPlaneBullet.spacing_2, b.pos.y } * gDisplayScale).Draw();
		texBrush.SetPosition(xx::XY{ b.pos.x + gPlaneBullet.spacing_2, b.pos.y } * gDisplayScale).Draw();
	});
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void ExplosionMonster::Init(xx::XY const& pos_, bool isBig) {
	pos = pos_;
	if (isBig) {
		frameIndex = gExplosionMonster.bigFrameIndexMin;
		Update = UpdateBig_();
		frames = &gLooper->frames_explosion_bigmonster;
	}
	else {
		frameIndex = gExplosionMonster.frameIndexMin;
		Update = Update_();
		frames = &gLooper->frames_explosion_monster;
	}
}
xx::Task<> ExplosionMonster::Update_() {
	while (true) {
		frameIndex += gExplosionMonster.frameSwitchDelay;
		if ((int)frameIndex > gExplosionMonster.frameIndexMax) break;
		co_yield 0;
	}
};
xx::Task<> ExplosionMonster::UpdateBig_() {
	while (true) {
		frameIndex += gExplosionMonster.frameSwitchDelay;
		if ((int)frameIndex > gExplosionMonster.bigFrameIndexMax) break;
		co_yield 0;
	}
};

void ExplosionMonster::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(frames->operator[](frameIndex)).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Bomb::Init(xx::XY const& pos_, BombTypes type_) {
	if (type_ == BombTypes::MAX_VALUE_UNKNOWN) {
		type = (BombTypes)gRnd.Next((int)BombTypes::MAX_VALUE_UNKNOWN - 1);
	}
	else {
		type = type_;
	}
	pos = pos_;
}
xx::Task<> Bomb::Update_() {
	do {
		pos.y -= gBomb.minSpeed;
		co_yield 0;
	} while (pos.y > -gDesign.height_2 - gBomb.diameter);
}
void Bomb::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_bomb[(int)type]).SetPosition(pos * gDisplayScale).Draw();
}


/*****************************************************************************************************/
/*****************************************************************************************************/

void MonsterStrawberry::Init() {
	if (gRnd.Next<bool>()) {
		pos.x = -gMonsterStrawberry.radius - gDesign.width_2;
		inc = { gMonsterStrawberry.horizontalMoveSpeed, 0 };
	}
	else {
		pos.x = gMonsterStrawberry.radius + gDesign.width_2;
		inc = { -gMonsterStrawberry.horizontalMoveSpeed, 0 };
	}
	pos.y = gRnd.Next(gMonsterStrawberry.bornYFrom, gMonsterStrawberry.bornYTo);
	frameIndex = gRnd.Next((float)gMonsterStrawberry.horizontalFrameIndexMin, gMonsterStrawberry.horizontalFrameIndexMax + 0.999f);
	switchDelay = gRnd.Next(gMonsterStrawberry.switchToVerticalMoveDelayFrom, gMonsterStrawberry.switchToVerticalMoveDelayTo);
}

xx::Task<> MonsterStrawberry::Update_() {
	// horizontal move
	while (true) {
		co_yield 0;
		pos += inc;
		if (--switchDelay <= 0) break;
		frameIndex += gMonsterStrawberry.horizontalMoveFrameSwitchDelay;
		if ((int)frameIndex > gMonsterStrawberry.horizontalFrameIndexMax) {
			frameIndex = gMonsterStrawberry.horizontalFrameIndexMin + (frameIndex - gMonsterStrawberry.horizontalFrameIndexMax - 1);
		}
	}

	// vertical move begin
	inc = { 0, -gMonsterStrawberry.verticalMoveSpeed };
	frameIndex = gMonsterStrawberry.verticalFrameIndexMin;
	while (pos.y > -gDesign.height_2 - gMonsterStrawberry.diameter) {
		co_yield 0;
		pos += inc;
		frameIndex += gMonsterStrawberry.verticalMoveFrameSwitchDelay;
		if ((int)frameIndex > gMonsterStrawberry.verticalFrameIndexMax) break;
	}

	// vertical move repeat
	frameIndex = gMonsterStrawberry.verticalRepeatFrameIndexMin + (frameIndex - gMonsterStrawberry.verticalFrameIndexMax - 1);
	while (pos.y > -gDesign.height_2 - gMonsterStrawberry.diameter) {
		co_yield 0;
		pos += inc;
		frameIndex += gMonsterStrawberry.horizontalMoveFrameSwitchDelay;
		if ((int)frameIndex > gMonsterStrawberry.verticalRepeatFrameIndexMax) {
			frameIndex = gMonsterStrawberry.verticalRepeatFrameIndexMin + (frameIndex - gMonsterStrawberry.verticalRepeatFrameIndexMax - 1);
		}
	}
};

void MonsterStrawberry::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_strawberry[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void MonsterDragonfly::Init(xx::MovePathCache* path_) {
	path = path_;
	frameIndex = gRnd.Next((float)gMonsterDragonfly.frameIndexMin, gMonsterDragonfly.frameIndexMax + 0.999f);
	pos = path->points[0].pos;
}

xx::Task<> MonsterDragonfly::Update_() {
	do {
		totalDistance += gMonsterDragonfly.speed;
		auto&& o = path->Move(totalDistance);
		if (!o) break;
		pos = o->pos;
		frameIndex += gMonsterDragonfly.frameSwitchDelay;
		if ((int)frameIndex > gMonsterDragonfly.frameIndexMax) {
			frameIndex = gMonsterDragonfly.frameIndexMin + (frameIndex - gMonsterDragonfly.frameIndexMax - 1);
		}
		co_yield 0;
	} while (pos.y > -gDesign.height_2 - gMonsterDragonfly.diameter);
};

void MonsterDragonfly::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_dragonfly[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void MonsterHermitCrab::Init() {
	frameIndex = gRnd.Next((float)gMonsterHermitCrab.frameIndexMin, gMonsterHermitCrab.frameIndexMax + 0.999f);
	pos.x = gRnd.Next(gMonsterHermitCrab.bornPosXFrom, gMonsterHermitCrab.bornPosXTo);
	pos.y = gMonsterHermitCrab.bornPosY;
}

xx::Task<> MonsterHermitCrab::Update_() {
	do {
		pos.y -= gMonsterHermitCrab.speed;
		frameIndex += gMonsterHermitCrab.frameSwitchDelay;
		if ((int)frameIndex > gMonsterHermitCrab.frameIndexMax) {
			frameIndex = gMonsterHermitCrab.frameIndexMin + (frameIndex - gMonsterHermitCrab.frameIndexMax - 1);
		}
		co_yield 0;
	} while (pos.y > -gDesign.height_2 - gMonsterHermitCrab.diameter);
};

void MonsterHermitCrab::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_hermit_crab[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

// todo
void MonsterFly::Init() {
	frameIndex = gRnd.Next((float)gMonsterFly.frameIndexMin, gMonsterFly.frameIndexMax + 0.999f);
}

xx::Task<> MonsterFly::Update_() {
	do {
		frameIndex += gMonsterFly.frameSwitchDelay;
		if ((int)frameIndex > gMonsterFly.frameIndexMax) {
			frameIndex = gMonsterFly.frameIndexMin + (frameIndex - gMonsterFly.frameIndexMax - 1);
		}
		co_yield 0;
	} while (pos.y > -gDesign.height_2 - gMonsterFly.diameter);
};

void MonsterFly::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_fly[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}
