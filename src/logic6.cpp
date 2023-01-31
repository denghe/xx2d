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
	auto halfSize = r.size / 2;

	float cx = std::abs(c.pos.x - r.pos.x);
	float xDist = halfSize.x + c.radius;
	if (cx > xDist)
		return false;
	float cy = std::abs(c.pos.y - r.pos.y);
	float yDist = halfSize.y + c.radius;
	if (cy > yDist)
		return false;
	if (cx <= halfSize.x || cy <= halfSize.y)
		return true;
	float xCornerDist = cx - halfSize.x;
	float yCornerDist = cy - halfSize.y;
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

	auto maxxy = rect.pos + rect.size / 2.0f;
	auto minxy = rect.pos - rect.size / 2.0f;

	do
	{
		auto diffx = circle.prePos.x - circle.pos.x;
		auto diffy = circle.prePos.y - circle.pos.y;

		bool crossx = (circle.pos.x >= minxy.x && circle.pos.x <= maxxy.x);
		bool crossy = (circle.pos.y >= minxy.y && circle.pos.y <= maxxy.y);

		if (crossx && crossy)
		{
			// 计算离哪条边最近
			float distancex, distancey;
			if (circle.pos.x >= rect.pos.x)
			{
				distancex = maxxy.x - (circle.pos.x + circle.radius);
			}
			else
			{
				distancex = (circle.pos.x - circle.radius) - minxy.x;
			}
			if (circle.pos.y >= rect.pos.y)
			{
				distancey = maxxy.y - (circle.pos.y + circle.radius);
			}
			else
			{
				distancey = (circle.pos.y - circle.radius) - minxy.y;
			}

			crossx = distancex > distancey;
			crossy = !crossx;
		}

		if (crossx)
		{
			if (diffy == 0.0f)
			{
				if (circle.pos.y >= rect.pos.y)
					circle.pos.y = maxxy.y + circle.radius;
				else
					circle.pos.y = minxy.y - circle.radius;
			}
			else if (diffy > 0.0f)
				circle.pos.y = maxxy.y + circle.radius;
			else
				circle.pos.y = minxy.y - circle.radius;

			break;
		}

		if (crossy)
		{
			if (diffx == 0.0f)
			{
				if (circle.pos.x >= rect.pos.x)
					circle.pos.x = maxxy.x + circle.radius;
				else
					circle.pos.x = minxy.x - circle.radius;
			}
			else if (diffx > 0.0f)
				circle.pos.x = maxxy.x + circle.radius;
			else
				circle.pos.x = minxy.x - circle.radius;

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
				crossPoint.x = minxy.x;
				crossPoint.y = minxy.y;
			}
			else
			{
				// 4
				crossPoint.x = minxy.x;
				crossPoint.y = maxxy.y;
			}
		}
		else
		{
			if (circle.pos.y < rect.pos.y)
			{
				// 2
				crossPoint.x = maxxy.x;
				crossPoint.y = minxy.y;
			}
			else
			{
				// 3
				crossPoint.x = maxxy.x;
				crossPoint.y = maxxy.y;
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

	for(auto i = 0; i < 100; ++i)
		cs.emplace_back().Init({(float)rnd.Next(-500, 500), (float)rnd.Next(-500, 500) }, 30, 64);
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
