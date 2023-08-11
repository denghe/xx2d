#include "main.h"
#ifdef _WIN32
#include <mmsystem.h>
#pragma comment(lib, "winmm")
namespace dummyxxx {
	inline int intval = []() {
		timeBeginPeriod(1);
		return 0;
	}();
}
#endif

int main() {
	auto g = std::make_unique<GameLooper>();
	gLooper = g.get();
	gEngine.w = gDesign.width * gDisplayScale;
	gEngine.h = gDesign.height * gDisplayScale;
	return g->Run("arcade: gemini wings");
}


/*****************************************************************************************************/
/*****************************************************************************************************/

// loop switch tex
template<float delay, int indexMin, int indexMax>
void StepFrameIndex(float& frameIndex) {
	frameIndex += delay;
	if ((int)frameIndex > indexMax) {
		frameIndex = indexMin + (frameIndex - indexMax - 1);
	}
}

template<float r1, float r2>
bool HitCheck(xx::XY const& p1, xx::XY const& p2) {
	auto d = p1 - p2;
	auto constexpr rr = (r1 + r2) * (r1 + r2);
	auto dd = d.x * d.x + d.y * d.y;
	return dd < rr;
}

template<typename MT>
std::optional<xx::XY> HitCheck(PlaneBullet& b, xx::ListLink<xx::Shared<MT>, int>& tar) {

	auto [idx, next] = tar.FindIf([&](xx::Shared<MT>& o)->bool {
		if (HitCheck<PlaneBullet::radius, MT::radius>(b.pos, o->pos)) {
			if constexpr (MT::radius > 8) {
				gLooper->explosions_bigmonster.Emplace().Emplace()->Init(o->pos);
			} else {
				gLooper->explosions_monster.Emplace().Emplace()->Init(o->pos);
			}
			return true;
		}
		return false;
		});
	if (idx != -1) {
		auto r = tar[idx]->pos;
		tar.Remove(idx, next);
		return r;
	}
	return {};
}

/*****************************************************************************************************/
/*****************************************************************************************************/

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
	tp.GetToByPrefix(frames_monster_bigfly, "monster_bigfly");
	tp.GetToByPrefix(frames_monster_butterfly, "monster_butterfly");
	tp.GetToByPrefix(frames_monster_clip, "monster_clip");

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

		monsters_bigfly.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		monsters_butterfly.Foreach([&](auto& o)->bool {
			return o->Update.Resume();
		});

		monsters_clip.Foreach([&](auto& o)->bool {
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

	monsters_bigfly.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	monsters_butterfly.Foreach([&](auto& o) {
		o->Draw(texBrush);
	});

	monsters_clip.Foreach([&](auto& o) {
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
	// reduce cpu usage
	if (xx::NowSteadyEpochSeconds() - gEngine.nowSecs < gFds / 2) {
		std::this_thread::sleep_for(std::chrono::milliseconds(int(gFds / 2 * 1000)));
	}
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

	//while (nowSecs < 10) {
	//	co_await gEngine.TaskSleep(1.f / 5);

	//	monsters_hermit_crab.Emplace().Emplace()->Init();
	//}
	//
	//xx::MovePathCache dragonflyPath1, dragonflyPath2;
	//{
	//	xx::MovePath mp;
	//	xx::CurvePoints cps;
	//	cps.points.emplace_back().pos = { g9Pos.x7 + 150, g9Pos.y7 + MonsterDragonfly::diameter };
	//	cps.points.emplace_back().pos = { g9Pos.x7 + 190, g9Pos.y7 - 10 };
	//	cps.points.emplace_back().pos = { g9Pos.x7 + 30, g9Pos.y7 - 100 };
	//	cps.points.emplace_back().pos = { g9Pos.x7 + 190, g9Pos.y7 - 170 };
	//	cps.points.emplace_back().pos = { g9Pos.x7 + 30, g9Pos.y7 - 250 };
	//	cps.points.emplace_back().pos = { g9Pos.x7 + 60, g9Pos.y3 - MonsterDragonfly::diameter };
	//	mp.Clear();
	//	mp.FillCurve(cps.isLoop, cps.points);
	//	dragonflyPath1.Init(mp, 1);

	//	cps.points.clear();
	//	cps.points.emplace_back().pos = { g9Pos.x9 - 150, g9Pos.y9 + MonsterDragonfly::diameter };
	//	cps.points.emplace_back().pos = { g9Pos.x9 - 190, g9Pos.y9 - 10 };
	//	cps.points.emplace_back().pos = { g9Pos.x9 - 30, g9Pos.y9 - 100 };
	//	cps.points.emplace_back().pos = { g9Pos.x9 - 190, g9Pos.y9 - 170 };
	//	cps.points.emplace_back().pos = { g9Pos.x9 - 30, g9Pos.y9 - 250 };
	//	cps.points.emplace_back().pos = { g9Pos.x9 - 60, g9Pos.y3 - MonsterDragonfly::diameter };
	//	mp.Clear();
	//	mp.FillCurve(cps.isLoop, cps.points);
	//	dragonflyPath2.Init(mp, 1);
	//}

	//while (nowSecs < 20) {
	//	co_await gEngine.TaskSleep(1.f / 10);

	//	// todo: if (plane is left  use path 1  else use path 2)
	//	monsters_dragonfly.Emplace().Emplace()->Init( gRnd.Next<bool>() ? &dragonflyPath1 : &dragonflyPath2);
	//}

	//while (nowSecs < 30) {
	//	co_await gEngine.TaskSleep(1.f / 10);

	//	monsters_strawberry.Emplace().Emplace()->Init();
	//}

	//while (nowSecs < 40) {
	//	co_await gEngine.TaskSleep(1.f / 20);

	//	monsters_fly.Emplace().Emplace()->Init();
	//}

	//while (nowSecs < 50) {
	//	co_await gEngine.TaskSleep(1.f / 5);

	//	monsters_bigfly.Emplace().Emplace()->Init();
	//}

	while (true) {
		co_await gEngine.TaskSleep(1.f / 5);

		monsters_clip.Emplace().Emplace()->Init();
	}

	// todo: create more monsters?
}

void Plane::Init(int planeIndex_) {
	// data init
	planeIndex = planeIndex_;
	speed = normalSpeed;
	frameIndex = frameIndexMid;
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
		float y = pos.y - Bomb::anchorYDist;
		for (size_t i = 0; i < n; i++) {
			bombs[i].pos = { pos.x, y - Bomb::diameter * i };
		}
	}
}

xx::Task<> Plane::SyncBombPos() {
	while (true) {
		co_yield 0;

		// eat bomb check
		gLooper->bombs.Foreach([this](auto& o)->bool {
			auto d = pos - o->pos;
			auto constexpr rr = (radius + Bomb::radius) * (radius + Bomb::radius);
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
		
		auto tarPos = xx::XY{ pos.x, pos.y - Bomb::anchorYDist };
		for (size_t i = 0; i < n; i++) {
			auto& pos = bombs[i].pos;

			auto d = tarPos - pos;
			auto dd = d.x * d.x + d.y * d.y;
			if (Bomb::minSpeedPow2 > dd) {
				pos = tarPos;
			}
			else {
				pos += d.As<float>() / (i ? (moving ? Bomb::movingFollowSteps : Bomb::stopFollowSteps) : Bomb::firstFollowSteps);
			}
			tarPos = { pos.x, pos.y - (moving ? 0 : Bomb::diameter) };
		}
	}
}

xx::Task<> Plane::SyncBulletPosCol() {
	while (true) {
		co_yield 0;

		StepFrameIndex<PlaneBullet::frameSwitchDelay, PlaneBullet::frameIndexMin, PlaneBullet::frameIndexMax>(bulletBeginFrameIndex);

		bullets.Foreach([](PlaneBullet& b)->bool {
			b.pos.y += PlaneBullet::speed;
			if (b.pos.y - PlaneBullet::hight_2 > gDesign.height_2) return true;	// flying out of the screen

			// todo: remove ::radius
			if (HitCheck(b, gLooper->monsters_strawberry).has_value()) return true;
			if (HitCheck(b, gLooper->monsters_dragonfly).has_value()) return true;
			if (HitCheck(b, gLooper->monsters_bigfly).has_value()) return true;
			if (HitCheck(b, gLooper->monsters_fly).has_value()) return true;
			if (HitCheck(b, gLooper->monsters_butterfly).has_value()) return true;
			if (HitCheck(b, gLooper->monsters_clip).has_value()) return true;
			if (auto pos = HitCheck(b, gLooper->monsters_hermit_crab); pos.has_value()) {
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
	pos = { bornXs[planeIndex], bornYFrom };
	InitBombPos();
	moving = true;
	while (bornYTo - pos.y > bornSpeed) {
		pos.y += bornSpeed;
		co_yield 0;
	}
	pos.y = bornYTo;
	
	tasks.Add(Update());			// switch to normal mode
}

xx::Task<> Plane::Shine() {
	auto e = gLooper->nowSecs + 5;
	do {
		visible += visibleInc;
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
			bulletNextFireTime = gLooper->nowSecs + PlaneBullet::fireCD;			// apply cd effect

			bullets.Emplace(xx::XY{ pos.x, pos.y + PlaneBullet::fireYOffset });
		}

		// use bomb
		if (gEngine.Pressed(xx::Mbtns::Right) && gLooper->nowSecs >= bulletNextFireTime) {
			bombNextUseTime = gLooper->nowSecs + PlaneBullet::fireCD;				// apply cd effect

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
		auto frameChangeSpeed = speed * 0.2;
		if (oldPos.x < pos.x) {
			frameIndex += frameChangeSpeed;
			if (frameIndex > frameIndexMax) {
				frameIndex = frameIndexMax;
			}
		}
		else if (oldPos.x > pos.x) {
			frameIndex -= frameChangeSpeed;
			if (frameIndex < frameIndexMin) {
				frameIndex = frameIndexMin;
			}
		}
		else {
			if (frameIndex > frameIndexMid) {
				frameIndex -= frameChangeSpeed;
				if (frameIndex < frameIndexMid) {
					frameIndex = frameIndexMid;
				}
			}
			else if (frameIndex < frameIndexMid) {
				frameIndex += frameChangeSpeed;
				if (frameIndex > frameIndexMid) {
					frameIndex = frameIndexMid;
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
		texBrush.SetPosition(xx::XY{ b.pos.x - PlaneBullet::spacing_2, b.pos.y } * gDisplayScale).Draw();
		texBrush.SetPosition(xx::XY{ b.pos.x + PlaneBullet::spacing_2, b.pos.y } * gDisplayScale).Draw();
	});
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Bomb::Init(xx::XY const& pos_, BombTypes type_) {
	this->Update = Update_();
	if (type_ == BombTypes::MAX_VALUE_UNKNOWN) {
		type = (BombTypes)gRnd.Next((int)BombTypes::MAX_VALUE_UNKNOWN - 1);
	} else {
		type = type_;
	}
	pos = pos_;
}
xx::Task<> Bomb::Update_() {
	do {
		pos.y -= minSpeed;
		co_yield 0;
	} while (pos.y > g9Pos.y1 - diameter);
}
void Bomb::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_bomb[(int)type]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void ExplosionMonster::Init(xx::XY const& pos_) {
	this->Update = Update_();
	pos = pos_;
	frameIndex = frameIndexMin;
}

xx::Task<> ExplosionMonster::Update_() {
	do {
		co_yield 0;
		frameIndex += frameSwitchDelay;
	} while ((int)frameIndex <= frameIndexMax);
};

void ExplosionMonster::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_explosion_monster[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}


void ExplosionBigMonster::Init(xx::XY const& pos_) {
	this->Update = Update_();
	pos = pos_;
	frameIndex = frameIndexMin;
}

xx::Task<> ExplosionBigMonster::Update_() {
	do {
		co_yield 0;
		frameIndex += frameSwitchDelay;
	} while ((int)frameIndex <= frameIndexMax);
};

void ExplosionBigMonster::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_explosion_bigmonster[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void MonsterStrawberry::Init() {
	this->Update = Update_();
	pos.x = gRnd.Next<bool>() ? (-radius - gDesign.width_2) : (radius + gDesign.width_2);
	pos.y = gRnd.Next(bornYFrom, bornYTo);
	frameIndex = gRnd.Next((float)horizontalFrameIndexMin, horizontalFrameIndexMax + 0.999f);
}

xx::Task<> MonsterStrawberry::Update_() {
	xx::XY inc{ pos.x < 0 ? horizontalMoveSpeed : -horizontalMoveSpeed, 0 };
	auto switchDelay = gRnd.Next(switchToVerticalMoveDelayFrom, switchToVerticalMoveDelayTo);
	// horizontal move
	do {
		co_yield 0;
		pos += inc;
		StepFrameIndex<frameSwitchDelay, horizontalFrameIndexMin, horizontalFrameIndexMax>(frameIndex);
	} while (--switchDelay >= 0);

	// vertical move begin
	inc = { 0, -verticalMoveSpeed };
	frameIndex = verticalFrameIndexMin;
	while (pos.y > g9Pos.y1 - diameter) {
		co_yield 0;
		pos += inc;
		frameIndex += verticalMoveFrameSwitchDelay;
		if ((int)frameIndex > verticalFrameIndexMax) break;
	}

	// vertical move repeat
	frameIndex = verticalRepeatFrameIndexMin + (frameIndex - verticalFrameIndexMax - 1);
	while (pos.y > g9Pos.y1 - diameter) {
		co_yield 0;
		pos += inc;
		StepFrameIndex<frameSwitchDelay, verticalRepeatFrameIndexMin, verticalRepeatFrameIndexMax>(frameIndex);
	}
};

void MonsterStrawberry::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_strawberry[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void MonsterDragonfly::Init(xx::MovePathCache* path_) {
	this->Update = Update_();
	path = path_;
	frameIndex = gRnd.Next((float)frameIndexMin, frameIndexMax + 0.999f);
	pos = path->points[0].pos;
}

xx::Task<> MonsterDragonfly::Update_() {
	float totalDistance{};
	do {
		totalDistance += speed;
		auto&& o = path->Move(totalDistance);
		if (!o) break;
		pos = o->pos;
		StepFrameIndex<frameSwitchDelay
			, frameIndexMin, frameIndexMax>(frameIndex);
		co_yield 0;
	} while (pos.y > g9Pos.y1 - diameter);
};

void MonsterDragonfly::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_dragonfly[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void MonsterHermitCrab::Init() {
	this->Update = Update_();
	frameIndex = gRnd.Next((float)frameIndexMin, frameIndexMax + 0.999f);
	pos.x = gRnd.Next(bornPosXFrom, bornPosXTo);
	pos.y = bornPosY;
}

xx::Task<> MonsterHermitCrab::Update_() {
	do {
		pos.y -= speed;
		StepFrameIndex<frameSwitchDelay, frameIndexMin, frameIndexMax>(frameIndex);
		co_yield 0;
	} while (pos.y > g9Pos.y1 - diameter);
};

void MonsterHermitCrab::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_hermit_crab[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void MonsterFly::Init() {
	this->Update = Update_();
	frameIndex = gRnd.Next((float)frameIndexMin, frameIndexMax + 0.999f);
	pos = { g9Pos.x7 + gRnd.Next<float>(gDesign.width - diameter), g9Pos.y7 + diameter };
}

xx::Task<> MonsterFly::Update_() {
	xx::XY tar{ g9Pos.x1 + gRnd.Next<float>(gDesign.width - diameter), g9Pos.y1 - diameter };
	auto d = tar - pos;
	auto dist = std::sqrt(d.x * d.x + d.y * d.y);
	auto speed = gRnd.Next(speedMin, speedMax);
	int lifeCycle = dist / speed;
	auto inc = d / lifeCycle;
	do {
		pos += inc;
		StepFrameIndex<frameSwitchDelay, frameIndexMin, frameIndexMax>(frameIndex);
		co_yield 0;
	} while (--lifeCycle >= 0);
};

void MonsterFly::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_fly[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void MonsterBigFly::Init() {
	frameIndex = gRnd.Next((float)frameIndexMin, frameIndexMax + 0.999f);
	pos.x = gRnd.Next<bool>() ? x1 : x2;
	pos.y = g9Pos.y7 + diameter;
}

xx::Task<> MonsterBigFly::Update_() {
	float xInc = pos.x == x1 ? speed : -speed;
	do {
		pos.y -= speed;
		StepFrameIndex<frameSwitchDelay1, frameIndexMin, frameIndexMax>(frameIndex);
		co_yield 0;
	} while (pos.y > horizontalMoveToY);

	do {
		pos.x += xInc;
		pos.y -= speed;
		StepFrameIndex<frameSwitchDelay2, frameIndexMin, frameIndexMax>(frameIndex);
		co_yield 0;
	} while (pos.y > g9Pos.y1 - diameter);
};

void MonsterBigFly::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_bigfly[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

// todo
void MonsterButterfly::Init() {
	this->Update = Update_();
	frameIndex = gRnd.Next((float)frameIndexMin, frameIndexMax + 0.999f);
}

xx::Task<> MonsterButterfly::Update_() {
	do {
		StepFrameIndex<frameSwitchDelay, frameIndexMin, frameIndexMax>(frameIndex);
		co_yield 0;
	} while (pos.y > g9Pos.y1 - diameter);
};

void MonsterButterfly::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_butterfly[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void MonsterClip::Init() {
	this->Update = Update_();
	frameIndex = gRnd.Next((float)frameIndexMin, frameIndexMax + 0.999f);
	pos = { gRnd.Next<float>(xFrom, xTo), g9Pos.y7 + diameter };
}

xx::Task<> MonsterClip::Update_() {
	do {
		pos.y -= speed;
		StepFrameIndex<frameSwitchDelay, frameIndexMin, frameIndexMax>(frameIndex);
		if (!gLooper->player_planes.empty()) {
			if (pos.y + aimPosYOffset <= gLooper->player_planes[0]->pos.y) goto LabVerticalMove;
		}
		co_yield 0;
	} while (pos.y > g9Pos.y1 - diameter);
	co_return;

LabVerticalMove:

	float delay = aimDelaySeconds;
	do {
		delay -= gFds;
		co_yield 0;
	} while (delay >= 0);
	
	if (pos.x > gLooper->player_planes[0]->pos.x) {
		frameIndex = verticalFrameIndexMin;
		do {
			pos.x -= speed;
			StepFrameIndex<frameSwitchDelay, verticalFrameIndexMin, verticalFrameIndexMax>(frameIndex);
			co_yield 0;
		} while (pos.x > g9Pos.x1 - diameter);
	}
	else {
		frameIndex = verticalFrameIndexMin2;
		do {
			pos.x += speed;
			StepFrameIndex<frameSwitchDelay, verticalFrameIndexMin2, verticalFrameIndexMax2>(frameIndex);
			co_yield 0;
		} while (pos.x < g9Pos.x3 + diameter);
	}
};

void MonsterClip::Draw(xx::Quad& texBrush) {
	texBrush.SetFrame(gLooper->frames_monster_clip[frameIndex]).SetPosition(pos * gDisplayScale).Draw();
}
