#pragma once
#include "engineMath.h"
#include <vector>

class CollisionComponent;

class Physics
{
public:
	class AABB
	{
	public:
		AABB() {}
		AABB(Vector3 min, Vector3 max)
			: mMin(min)
			, mMax(max)
		{}

		Vector3 mMin;
		Vector3 mMax;
	};

	class LineSegment
	{
	public:
		LineSegment() {}
		LineSegment(Vector3 start, Vector3 end)
			: mStart(start)
			, mEnd(end)
		{}

		Vector3 mStart;
		Vector3 mEnd;
	};

	static bool Intersect(const AABB& a, const AABB& b, AABB* pOverlap = nullptr);
	static bool Intersect(const LineSegment& segment, const AABB& box, Vector3* pHitPoint = nullptr);
	static bool UnitTest();

	void AddObj(CollisionComponent* pObj);
	void RemoveObj(CollisionComponent* pObj);

	bool RayCast(const LineSegment& segment, Vector3* pHitPoint = nullptr);

private:
	std::vector<CollisionComponent*> mCollisions;
};

