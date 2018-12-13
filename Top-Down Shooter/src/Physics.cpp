#include "Physics.h"
#include "Components.h"

// Find the overlap between two entities' bounding boxes
Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	Vec2 firstbox = a->getComponent<CBoundingBox>()->halfSize;
	Vec2 secondbox = b->getComponent<CBoundingBox>()->halfSize;

	Vec2 firstpos = a->getComponent<CTransform>()->pos;
	Vec2 secondpos = b->getComponent<CTransform>()->pos;

	Vec2 dist = Vec2(abs(firstpos.x - secondpos.x), abs(firstpos.y - secondpos.y));

	float ox = firstbox.x + secondbox.x - dist.x;
	float oy = firstbox.y + secondbox.y - dist.y;

	return Vec2(ox, oy);
}

// Find the previous overlap between two entities' bounding boxes
Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	Vec2 firstbox = a->getComponent<CBoundingBox>()->halfSize;
	Vec2 secondbox = b->getComponent<CBoundingBox>()->halfSize;

	Vec2 firstpos = a->getComponent<CTransform>()->prevPos;
	Vec2 secondpos = b->getComponent<CTransform>()->prevPos;

	Vec2 dist = Vec2(abs(firstpos.x - secondpos.x), abs(firstpos.y - secondpos.y));

	float ox = firstbox.x + secondbox.x - dist.x;
	float oy = firstbox.y + secondbox.y - dist.y;

	return Vec2(ox, oy);
}

// Determine if two lines intersect, and if so, find the point of intersection
Intersect Physics::LineIntersect(const Vec2 & a, const Vec2 & b, const Vec2 & c, const Vec2 & d)
{    
	Vec2 r = b - a;
	Vec2 s = d - c;
	float rxs = r.x * s.y - r.y * s.x;
	Vec2 cma = c - a;
	float t = (cma.x * s.y - cma.y * s.x) / rxs;
	float u = (cma.x * r.y - cma.y * r.x) / rxs;
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
	{
		return { true, Vec2(a.x + t * r.x, a.y + t * r.y) };
	}
	else
	{
		return { false, Vec2(0,0) };
	}
}

// Determine if a line intersects an entity's bounding box
bool Physics::EntityIntersect(const Vec2 & a, const Vec2 & b, std::shared_ptr<Entity> e)
{
	Vec2 topLeft = e->getComponent<CTransform>()->pos - Vec2(e->getComponent<CBoundingBox>()->halfSize.x, e->getComponent<CBoundingBox>()->halfSize.y);
	Vec2 topRight = e->getComponent<CTransform>()->pos - Vec2(-e->getComponent<CBoundingBox>()->halfSize.x, e->getComponent<CBoundingBox>()->halfSize.y);
	Vec2 bottomLeft = e->getComponent<CTransform>()->pos - Vec2(e->getComponent<CBoundingBox>()->halfSize.x, -e->getComponent<CBoundingBox>()->halfSize.y);
	Vec2 bottomRight = e->getComponent<CTransform>()->pos - Vec2(-e->getComponent<CBoundingBox>()->halfSize.x, -e->getComponent<CBoundingBox>()->halfSize.y);
	
	Intersect topEdge = LineIntersect(a, b, topLeft, topRight);
	Intersect leftEdge = LineIntersect(a, b, topLeft, bottomLeft);
	Intersect bottomEdge = LineIntersect(a, b, bottomLeft, bottomRight);
	Intersect rightEdge = LineIntersect(a, b, bottomRight, topRight);

	return(topEdge.result || bottomEdge.result || leftEdge.result || rightEdge.result);
}