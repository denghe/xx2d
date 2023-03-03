#include "xx2d_pch.h"
#include "game_looper.h"
#include "s7_more_box_circle_cd.h"

namespace MoreBoxCircleCD {

	void C::Init(Scene* const& owner_, xx::Pos<> const& pos, int32_t const& r, int32_t const& segments) {
		owner = owner_;
		radius = r;

		SGCInit(&owner->sgc);
		SGCSetPos(pos);
		SGCAdd();

		border.FillCirclePoints({ 0,0 }, radius, {}, segments)
			.SetColor({ 255, 255, 0, 255 })
			.SetPosition({ (float)pos.x, (float)-pos.y });
	}
	void C::SetPos(xx::Pos<> const& pos) {
		SGCSetPos(pos);
		SGCUpdate();

		border.SetPosition({ (float)pos.x, (float)-pos.y });
	}
	void C::Update() {
		int foreachLimit = 100, numCross{};
		xx::Pos<float> v{};	// combine force vector
		newPos = _sgcPos;

		// calc v
		_sgc->Foreach9NeighborCells<true>(this, [&](C* const& c) {
			assert(c != this);
			// prepare cross check. (r1 + r2) * (r1 + r2) > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
			auto rr = (c->radius + this->radius) * (c->radius + this->radius);
			auto d = _sgcPos - c->_sgcPos;
			auto dd = d.x * d.x + d.y * d.y;
			// cross?
			if (rr > dd) {
				++numCross;
				if (dd) {
					v += d.As<float>() / std::sqrt(float(dd));
				}
			}
			}, &foreachLimit);

		auto d = owner->mousePos - _sgcPos;
		auto dd = d.x * d.x + d.y * d.y;

		// cross?
		if (numCross) {
			// simulate move to mouse pos force
			if (dd) {
				v += d.As<float>() / std::sqrt(float(dd)) / 100;
			}
			if (v.IsZero()) {	// move by random angle
				v = xx::Rotate(xx::Pos<>{ speed, 0 }, owner->rnd.Next() % xx::table_num_angles).As<float>();
			} else {	// move by v
				v = v.Normalize() * speed;
			}
			newPos += v + 0.5f;	// fix float -> int problem
		} else {
			if (dd > speed * 1.5) {
				v = d.As<float>() / std::sqrt(float(dd)) * speed;
				newPos += v + 0.5f;	// fix float -> int problem
			} else {
				newPos = owner->mousePos;
			}
		}

		// get box & fix newPos
		auto minXY = newPos - radius, maxXY = newPos + radius;
		if (minXY.x < 0) minXY.x = 0;
		if (minXY.y < 0) minXY.y = 0;
		if (maxXY.x >= owner->sgab.maxX) maxXY.x = owner->sgab.maxX - 1;
		if (maxXY.y >= owner->sgab.maxY) maxXY.y = owner->sgab.maxY - 1;
		owner->sgab.ForeachAABB(minXY, maxXY);
		for (auto& b : owner->sgab.results) {
			xx::MoveCircleIfIntersectsBox(b->_sgabPos.x, b->_sgabPos.y, b->size.x / 2, b->size.y / 2, newPos.x, newPos.y, radius);
		}
		owner->sgab.ClearResults();

		// map edge limit
		if (newPos.x < 0) newPos.x = 0;
		else if (newPos.x >= _sgc->maxX) newPos.x = _sgc->maxX - 1;
		if (newPos.y < 0) newPos.y = 0;
		else if (newPos.y >= _sgc->maxY) newPos.y = _sgc->maxY - 1;
	}
	void C::Update2() {
		if (_sgcPos != newPos) {
			_sgcPos = newPos;
			_sgc->Update(this);
			border.SetColor({ 255, 0, 0, 255 })
				.SetPosition({ (float)_sgcPos.x, (float)-_sgcPos.y });
			++_sgc->numActives;
		} else {
			if (border.color != xx::RGBA8{ 255, 255, 255, 255 }) {
				border.SetColor({ 255, 255, 255, 255 });
			}
		}
	}
	C::~C() {
		SGCRemove();
	}


	void B::Init(Scene* const& owner_, xx::Pos<> const& pos, xx::Pos<> const& siz) {
		owner = owner_;
		size = siz;

		SGABInit(&owner->sgab);
		SGABSetPosSiz(pos, siz);
		SGABAdd();

		border.FillBoxPoints({}, siz.As<float>())
			.SetColor({ 0, 255, 0, 255 })
			.SetPosition({ (float)pos.x, (float)-pos.y });
	}
	B::~B() {
		SGABRemove();
	}


	void Scene::Init(GameLooper* looper) {
		this->looper = looper;

		std::cout << "MoreBoxCircleCD::Scene::Init" << std::endl;

		sgc.Init(400, 400, 64);
		sgab.Init(400, 400, 64, 64);

		cam.Init({ xx::engine.w, xx::engine.h }, &sgc);
		cam.SetPosition({ sgc.maxX / 2.f, sgc.maxY / 2.f });
		cam.SetScale(0.15);
		cam.Commit();

		//cs.emplace_back().Emplace()->Init(this, { sgc.maxX / 2, sgc.maxY / 2 }, 32, 16);
		for (auto i = 0; i < 10000; ++i) {
			auto r = rnd.Next(16, 32);
			xx::Pos<> v{ rnd.Next(r, sgc.maxX1 - r), rnd.Next(r, sgc.maxY1 - r) };
			cs.emplace_back().Emplace()->Init(this, v, r, 16);
		}

		auto cx = sgc.maxX / 2, cy = sgc.maxY / 2;
		bs.emplace_back().Emplace()->Init(this, { cx, cy + 1000 }, { 2000, 200 });
		bs.emplace_back().Emplace()->Init(this, { cx - 1000, cy }, { 200, 2000 });
		bs.emplace_back().Emplace()->Init(this, { cx + 1000, cy }, { 200, 2000 });
		bs.emplace_back().Emplace()->Init(this, { cx - 800, cy - 1000 }, { 400, 200 });
		bs.emplace_back().Emplace()->Init(this, { cx + 800, cy - 1000 }, { 400, 200 });

		mousePos.Set(cam.pos);
	}

	int Scene::Update() {
		if (xx::engine.Pressed(xx::Mbtns::Left)) {
			mousePos.Set(cam.pos + xx::engine.mousePosition.MakeFlipY() / cam.scale);
		}

		timePool += xx::engine.delta;
		auto timePoolBak = timePool;
		if (timePool >= 1.f / 360) {
			timePool = 0;

			for (auto& c : cs) {
				c->Update();
			}
			for (auto& c : cs) {
				c->Update2();
			}
		}

		for (auto& c : cs) {
			c->border.Draw(cam.at);
		}
		for (auto& b : bs) {
			b->border.Draw(cam.at);
		}

		return 0;
	}

}
