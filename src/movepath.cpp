#include "pch.h"

namespace xx {

	void MovePath::Fill(xx::XY const* ps, size_t len, bool loop) {
		assert(len > 1);
		totalDistance = {};
		this->loop = loop;
		if (ps) {
			points.resize(len);
			for (size_t i = 0; i < len; i++) {
				points[i].pos = ps[i];
			}
		} else {
			assert(len <= points.size());
		}
		for (size_t i = 0; i < len - 1; i++) {
			FillFields(points[i], points[i + 1]);
			totalDistance += points[i].distance;
		}
		if (loop) {
			FillFields(points[len - 1], points[0]);
			totalDistance += points[len - 1].distance;
		} else {
			points[len - 1].distance = {};
			points[len - 1].inc = {};
			points[len - 1].radians = points[len - 2].radians;
		}
	}

	void MovePath::Fill(bool loop) {
		Fill(nullptr, points.size(), loop);
	}

	void MovePath::FillFields(MovePathPoint& p1, MovePathPoint& p2) {
		auto v = p2.pos - p1.pos;
		p1.radians = std::atan2(v.y, v.x);
		p1.inc = { std::cos(p1.radians), std::sin(p1.radians) };
		p1.distance = std::sqrt(v.x * v.x + v.y * v.y);
	}

	// pathway curve ( p p p ... )  to 2 control points bezier( p c c p c c p ... )
	template<typename List1, typename List2>
	inline void CurveToBezier(List1& bs, List2 const& cs) {
		auto n = cs.size();
		auto len = n * 3 - 2;
		bs.resize(len);

		bs[0] = cs[0];
		bs[1] = (cs[1] - cs[0]) * cs[0].tension + cs[0];

		for (size_t i = 0; i < n - 2; i++) {
			auto diff = (cs[i + 2] - cs[i]) * cs[i].tension;
			bs[3 * i + 2] = cs[i + 1] - diff;
			bs[3 * i + 3] = cs[i + 1];
			bs[3 * i + 4] = cs[i + 1] + diff;
		}
		bs[len - 2] = (cs[n - 2] - cs[n - 1]) * cs[n - 2].tension + cs[n - 1];
		bs[len - 1] = cs[n - 1];
	}

	// 2 control points bezier( p c c p c c p ... ) to split points
	template<typename List1, typename List2>
	inline void BezierToPoints(List1& ps, List2 const& bs) {
		auto len = (bs.size() - 1) / 3;
		size_t totalSegments = 0;
		for (size_t j = 0; j < len; ++j) {
			totalSegments += bs[j * 3].numSegments;
		}
		ps.resize(totalSegments);
		totalSegments = 0;
		for (size_t j = 0; j < len; ++j) {
			auto idx = j * 3;
			auto numSegments = bs[idx].numSegments;
			auto step = 1.0f / numSegments;
			for (int i = 0; i < numSegments; ++i) {
				auto t = step * i;
				auto t1 = 1 - t;
				ps[totalSegments + i] = bs[idx] * t1 * t1 * t1
					+ bs[idx + 1] * 3 * t * t1 * t1
					+ bs[idx + 2] * 3 * t * t * (1 - t)
					+ bs[idx + 3] * t * t * t;
			}
			totalSegments += numSegments;
		}
	}

	void MovePath::FillCurve(bool loop, std::vector<CurvePoint> const& ps) {
		auto len = ps.size();
		assert(len >= 2);

		// 2 point: line
		if (len == 2) {
			points.emplace_back(ps[0].pos);
			points.emplace_back(ps[1].pos);
		}
		// curve
		else {
			std::vector<CurvePoint> cs;

			// curve to 2 control points curve
			if (loop) {
				std::vector<CurvePoint> bs;
				bs.reserve(len + 6);
				// insert addons
				bs.push_back(ps[len - 3]);
				bs.push_back(ps[len - 2]);
				bs.push_back(ps[len - 1]);
				bs.insert(bs.end(), ps.begin(), ps.end());
				bs.push_back(ps[0]);
				bs.push_back(ps[1]);
				bs.push_back(ps[2]);
				CurveToBezier(cs, bs);
				// remove addons
				cs.resize(cs.size() - 6);
				cs.erase(cs.begin(), cs.begin() + 9);
			} else {
				CurveToBezier(cs, ps);
			}

			// 2 control points curve to split points
			BezierToPoints(points, cs);

			if (!loop) {
				auto& last = ps.back();
				auto& point = points.emplace_back();
				point.pos = last.pos;
			}

			// fill inc, radians, distance
			Fill(loop);
		}
	}

	void MovePathSteper::Init(xx::Shared<MovePath> mp) {
		this->mp = std::move(mp);
		cursor = {};
		cursorDistance = {};
	}

	MovePathSteper::MoveResult MovePathSteper::MoveToBegin() {
		assert(mp);
		assert(mp->points.size());
		cursor = {};
		cursorDistance = {};
		auto& p = mp->points.front();
		return { .pos = p.pos, .radians = p.radians, .movedDistance = {}, .terminated = false };
	}

	MovePathSteper::MoveResult MovePathSteper::MoveForward(float const& stepDistance) {
		assert(mp);
		assert(mp->points.size());
		auto& ps = mp->points;
		auto siz = ps.size();
		auto loop = mp->loop;
		auto d = stepDistance;
	LabLoop:
		auto& p = ps[cursor];
		auto left = p.distance - cursorDistance;
		if (d > left) {
			d -= left;
			cursorDistance = 0.f;
			++cursor;
			if (cursor == siz) {
				if (loop) {
					cursor = 0;
				} else {
					cursor = siz - 1;
					return { .pos = p.pos, .radians = p.radians, .movedDistance = stepDistance - d, .terminated = true };
				}
			}
			goto LabLoop;
		} else {
			cursorDistance += d;
		}
		return { .pos = p.pos + (p.inc * cursorDistance), .radians = p.radians, .movedDistance = stepDistance, .terminated = !mp->loop && cursor == siz - 1 };
	}

	void MovePathCache::Init(xx::Shared<MovePath> mp, float const& stepDistance) {
		assert(mp);
		assert(stepDistance > 0);
		assert(mp->totalDistance > stepDistance);
		this->stepDistance = stepDistance;
		this->loop = mp->loop;
		auto td = mp->totalDistance + stepDistance;
		points.clear();
		points.reserve(std::ceil(mp->totalDistance / stepDistance));
		MovePathSteper mpr;
		mpr.Init(std::move(mp));
		auto mr = mpr.MoveToBegin();
		points.push_back({ mr.pos, mr.radians });
		for (float d = stepDistance; d < td; d += stepDistance) {
			mr = mpr.MoveForward(stepDistance);
			points.push_back({ mr.pos, mr.radians });
		}
	}

	MovePathCachePoint* MovePathCache::Move(float const& totalDistance) {
		int i = totalDistance / stepDistance;
		if (loop) {
			return &points[i % points.size()];
		} else {
			return i < points.size() ? &points[i] : nullptr;
		}
	}
}
