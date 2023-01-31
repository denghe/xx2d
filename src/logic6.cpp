#include "pch.h"
#include "logic.h"
#include "logic6.h"

inline float XYLengthSq(const XY& pt)
{
	return pt.x * pt.x + pt.y * pt.y;
}

inline void XYNormal(XY& pt)
{
	auto lengthSq = XYLengthSq(pt);
	if (lengthSq == 0.0f)
	{
		pt.x = 1.0f;
		pt.y = 0.0f;
	}
	else
	{
		lengthSq = ::sqrt(lengthSq);
		pt.x = pt.x / lengthSq;
		pt.y = pt.y / lengthSq;
	}
}

bool CircleToBox(const DragCircle& c, const DragBox& r)
{
	float cx = std::abs(c.pos.x - r.pos.x);
	float xDist = r.hs.x + c.radius;
	if (cx > xDist)
		return false;
	float cy = std::abs(c.pos.y - r.pos.y);
	float yDist = r.hs.y + c.radius;
	if (cy > yDist)
		return false;
	if (cx <= r.hs.x || cy <= r.hs.y)
		return true;
	float xCornerDist = cx - r.hs.x;
	float yCornerDist = cy - r.hs.y;
	float xCornerDistSq = xCornerDist * xCornerDist;
	float yCornerDistSq = yCornerDist * yCornerDist;
	float maxCornerDistSq = c.radius * c.radius;
	return xCornerDistSq + yCornerDistSq <= maxCornerDistSq;
}


void CollisionDetection(DragBox& rect, DragCircle& circle)
{
	if (!CircleToBox(circle, rect))
	{
		rect.border.SetColor({ 0, 255, 0, 255 });
		rect.border.Commit();
		circle.border.SetColor({ 255, 255, 0, 255 });
		circle.border.Commit();
		return;
	}

	do
	{
		auto diffx = circle.prePos.x - circle.pos.x;
		auto diffy = circle.prePos.y - circle.pos.y;

		bool crossx = (circle.pos.x >= rect.Minx() && circle.pos.x <= rect.Maxx());
		bool crossy = (circle.pos.y >= rect.Miny() && circle.pos.y <= rect.Maxy());

		if (crossx && crossy)
		{
			// 计算离哪条边最近
			float distancex, distancey;
			if (circle.pos.x >= rect.pos.x)
			{
				distancex = rect.Maxx() - (circle.pos.x + circle.radius);
			}
			else
			{
				distancex = (circle.pos.x - circle.radius) - rect.Minx();
			}
			if (circle.pos.y >= rect.pos.y)
			{
				distancey = rect.Maxy() - (circle.pos.y + circle.radius);
			}
			else
			{
				distancey = (circle.pos.y - circle.radius) - rect.Miny();
			}

			crossx = distancex > distancey;
			crossy = !crossx;
		}

		if (crossx)
		{
			if (diffy == 0.0f)
			{
				if (circle.pos.y >= rect.pos.y)
					circle.pos.y = rect.Maxy() + circle.radius;
				else
					circle.pos.y = rect.Miny() - circle.radius;
			}
			else if (diffy > 0.0f)
				circle.pos.y = rect.Maxy() + circle.radius;
			else
				circle.pos.y = rect.Miny() - circle.radius;

			break;
		}

		if (crossy)
		{
			if (diffx == 0.0f)
			{
				if (circle.pos.x >= rect.pos.x)
					circle.pos.x = rect.Maxx() + circle.radius;
				else
					circle.pos.x = rect.Minx() - circle.radius;
			}
			else if (diffx > 0.0f)
				circle.pos.x = rect.Maxx() + circle.radius;
			else
				circle.pos.x = rect.Minx() - circle.radius;

			break;
		}


		///*
		//1	________________	2
		//	|				|
		//	|			    |
		//	|               |
		//	|_______________|
		//4						3
		//*/
		XY crossPoint;
		if (circle.pos.x < rect.pos.x)
		{
			if (circle.pos.y < rect.pos.y)
			{
				// 1
				crossPoint.x = rect.Minx();
				crossPoint.y = rect.Miny();
			}
			else
			{
				// 4
				crossPoint.x = rect.Minx();
				crossPoint.y = rect.Maxy();
			}
		}
		else
		{
			if (circle.pos.y < rect.pos.y)
			{
				// 2
				crossPoint.x = rect.Maxx();
				crossPoint.y = rect.Miny();
			}
			else
			{
				// 3
				crossPoint.x = rect.Maxx();
				crossPoint.y = rect.Maxy();
			}
		}

		XY pt = circle.pos - crossPoint;
		XYNormal(pt);

		pt.x *= circle.radius;
		pt.y *= circle.radius;

		circle.pos.x = crossPoint.x + pt.x;
		circle.pos.y = crossPoint.y + pt.y;

	} while (false);


	rect.border.SetColor({ 255, 0, 0, 255 });
	rect.border.Commit();

	circle.border.SetColor({ 255, 0, 0, 255 });
	circle.border.SetPositon(circle.pos);
	circle.border.Commit();
}

void Logic6::Init(Logic* eg) {
	this->eg = eg;

	std::cout << "Logic6 Init( test box + circle collision detect )" << std::endl;

	cs.emplace_back().Init({10, 0}, 30, 64);
	bs.emplace_back().Init({50, 0}, {50, 200});

	BL.Init(eg, Mbtns::Left);
	CL.Init(eg, Mbtns::Right);
}

int Logic6::Update() {

	{
		CL.Update();
		auto&& iter = cs.begin();
		while (CL.eventId && iter != cs.end()) {
			CL.Dispatch(&*iter++);
		}
	}
	{
		BL.Update();
		auto&& iter = bs.begin();
		while (BL.eventId && iter != bs.end()) {
			BL.Dispatch(&*iter++);
		}
	}

	for (auto& b : bs)
	{
		for (auto& c : cs)
		{
			CollisionDetection(b, c);
		}
	}

	for (auto& c : cs) {
		c.border.Draw(eg);
		c.prePos = c.pos;
	}
	for (auto& b : bs) {
		b.border.Draw(eg);
	}

	return 0;
}
