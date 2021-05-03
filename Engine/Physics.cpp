#include "stdafx.h"
#include "Physics.h"
#include "Components\collisionComponent.h"


/*static*/ bool Physics::Intersect(const AABB& a, const AABB& b, AABB* pOverlap)
{
    AABB overlap;
    overlap.mMin.x = max(a.mMin.x, b.mMin.x);
    overlap.mMin.y = max(a.mMin.y, b.mMin.y);
    overlap.mMin.z = max(a.mMin.z, b.mMin.z);
    overlap.mMax.x = min(a.mMax.x, b.mMax.x);
    overlap.mMax.y = min(a.mMax.y, b.mMax.y);
    overlap.mMax.z = min(a.mMax.z, b.mMax.z);
    if (nullptr != pOverlap)
        *pOverlap = overlap;
    return overlap.mMin.x < overlap.mMax.x
        && overlap.mMin.y < overlap.mMax.y
        && overlap.mMin.z < overlap.mMax.z;
}

/*static*/ bool Physics::Intersect(const LineSegment& segment, const AABB& box, Vector3* pHitPoint)
{
    static const float s_closeEnuf = 0.001f;
    Vector3 d = segment.mEnd - segment.mStart;
    float tmin = -FLT_MAX; // set to -FLT_MAX to get first hit on line
    float tmax = FLT_MAX; // set to max distance ray can travel (for segment)

                          // x axis
    if (fabsf(d.x) < s_closeEnuf)
    {
        // LineSegment is parallel to slab. No hit if origin not within slab
        if (segment.mStart.x < box.mMin.x || segment.mStart.x > box.mMax.x)
            return false;
    }
    else
    {
        float ood = 1.0f / d.x;
        float t1 = (box.mMin.x - segment.mStart.x) * ood;
        float t2 = (box.mMax.x - segment.mStart.x) * ood;
        // Make t1 be intersection with near plane, t2 with far plane
        if (t1 > t2)
        {
            float temp = t2;
            t2 = t1;
            t1 = temp;
        }
        // Compute the intersection of slab intersection intervals
        tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
        tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
                              // Exit with no collision as soon as slab intersection becomes empty
        if (tmin > tmax)
            return false;
    }

    // y axis
    if (fabsf(d.y) < s_closeEnuf)
    {
        // LineSegment is parallel to slab. No hit if origin not within slab
        if (segment.mStart.y < box.mMin.y || segment.mStart.y > box.mMax.y)
            return false;
    }
    else
    {
        float ood = 1.0f / d.y;
        float t1 = (box.mMin.y - segment.mStart.y) * ood;
        float t2 = (box.mMax.y - segment.mStart.y) * ood;
        // Make t1 be intersection with near plane, t2 with far plane
        if (t1 > t2)
        {
            float temp = t2;
            t2 = t1;
            t1 = temp;
        }
        // Compute the intersection of slab intersection intervals
        tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
        tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
                              // Exit with no collision as soon as slab intersection becomes empty
        if (tmin > tmax)
            return false;
    }

    // z axis
    if (fabsf(d.z) < s_closeEnuf)
    {
        // LineSegment is parallel to slab. No hit if origin not within slab
        if (segment.mStart.z < box.mMin.z || segment.mStart.z > box.mMax.z)
            return false;
    }
    else
    {
        float ood = 1.0f / d.z;
        float t1 = (box.mMin.z - segment.mStart.z) * ood;
        float t2 = (box.mMax.z - segment.mStart.z) * ood;
        // Make t1 be intersection with near plane, t2 with far plane
        if (t1 > t2)
        {
            float temp = t2;
            t2 = t1;
            t1 = temp;
        }
        // Compute the intersection of slab intersection intervals
        tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
        tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
                              // Exit with no collision as soon as slab intersection becomes empty
        if (tmin > tmax)
            return false;
    }

    if (tmin > 1.0f || tmax < 0.0f)
        return false;

    // LineSegment intersects all 3 slabs. Return point (q) and intersection t value (tmin)
    if (nullptr != pHitPoint)
        *pHitPoint = segment.mStart + d * tmin;

    return true;
}

/*static*/ bool Physics::UnitTest()
{
    struct TestAABB
    {
        AABB    a;
        AABB    b;
        AABB    overlap;
    };
    const TestAABB testAABB[] =
    {
        {
            AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f)),
            AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-110.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f)),
            AABB(Vector3(-100.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(110.0f, 10.0f, 10.0f)),
            AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(100.0f, 10.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -110.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f)),
            AABB(Vector3(-10.0f, -100.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 110.0f, 10.0f)),
            AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 100.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -10.0f, -110.0f), Vector3(10.0f, 10.0f, -90.0f)),
            AABB(Vector3(-10.0f, -10.0f, -100.0f), Vector3(10.0f, 10.0f, -90.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 110.0f)),
            AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 100.0f))
        },

        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-120.0f, -10.0f, -10.0f), Vector3(-110.0f, 10.0f, 10.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(110.0f, -10.0f, -10.0f), Vector3(120.0f, 10.0f, 10.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -120.0f, -10.0f), Vector3(10.0f, -110.0f, 10.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, 110.0f, -10.0f), Vector3(10.0f, 120.0f, 10.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -10.0f, -120.0f), Vector3(10.0f, 10.0f, -110.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -10.0f, 110.0f), Vector3(10.0f, 10.0f, 120.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
    };

    bool isOk = true;
    for (TestAABB test : testAABB)
    {
        AABB overlap;
        bool hit = Intersect(test.a, test.b, &overlap);
        if (test.overlap.mMin.x < test.overlap.mMax.x)
        {   // this test should hit
            isOk &= hit;
            isOk &= Math::IsCloseEnuf(overlap.mMin, test.overlap.mMin);
            isOk &= Math::IsCloseEnuf(overlap.mMax, test.overlap.mMax);
        }
        else
        {
            isOk &= (false == hit);
        }
    }

    struct TestSegment
    {
        AABB        box;
        LineSegment segment;
        bool        hit;
        Vector3     point;
    };
    const TestSegment testSegment[] =
    {
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(-110.0f, 0.0f, 0.0f), Vector3(-90.0f, 0.0f, 0.0f)),
            true,   Vector3(-100.0f, 0.0f, 0.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(0.0f, -110.0f, 0.0f), Vector3(0.0f, -90.0f, 0.0f)),
            true,   Vector3(0.0f, -100.0f, 0.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(0.0f, 0.0f, -110.0f), Vector3(0.0f, 0.0f, -90.0f)),
            true,   Vector3(0.0f, 0.0f, -100.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(110.0f, 0.0f, 0.0f), Vector3(90.0f, 0.0f, 0.0f)),
            true,   Vector3(100.0f, 0.0f, 0.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(0.0f, 110.0f, 0.0f), Vector3(0.0f, 90.0f, 0.0f)),
            true,   Vector3(0.0f, 100.0f, 0.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(0.0f, 0.0f, 110.0f), Vector3(0.0f, 0.0f, 90.0f)),
            true,   Vector3(0.0f, 0.0f, 100.0f)
        },

        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(-120.0f, 0.0f, 0.0f), Vector3(-110.0f, 0.0f, 0.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(0.0f, -120.0f, 0.0f), Vector3(0.0f, -110.0f, 0.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(0.0f, 0.0f, -120.0f), Vector3(0.0f, 0.0f, -110.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(120.0f, 0.0f, 0.0f), Vector3(110.0f, 0.0f, 0.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(0.0f, 120.0f, 0.0f), Vector3(0.0f, 110.0f, 0.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            LineSegment(Vector3(0.0f, 0.0f, 120.0f), Vector3(0.0f, 0.0f, 110.0f)),
            false,  Vector3::Zero
        },
    };

    for (TestSegment test : testSegment)
    {
        Vector3 hitPoint;
        bool hit = Intersect(test.segment, test.box, &hitPoint);
        isOk &= (hit == test.hit);
        if (test.hit)
            isOk &= Math::IsCloseEnuf(hitPoint, test.point);
        DbgAssert(isOk, "lame");
    }

	return isOk;
}

void Physics::AddObj(CollisionComponent* pObj)
{
    mCollisions.push_back(pObj);
}

void Physics::RemoveObj(CollisionComponent* pObj)
{
    for (auto it = mCollisions.begin(); it != mCollisions.end(); ++it)
    {
        CollisionComponent* pFind = *it;
        if (pFind == pObj)
        {
            mCollisions.erase(it);
            return;
        }
    }
}

bool Physics::RayCast(const LineSegment& segment, Vector3* pHitPoint)
{
    bool hit = false;
    float bestDist = 1e20f;
    for (const auto pObj : mCollisions)
    {
        Vector3 point;
        if (Intersect(segment, pObj->GetAABB(), &point))
        {
            hit = true;
            float distSq = (point - segment.mStart).LengthSq();
            if (distSq < bestDist)
            {
                bestDist = distSq;
                *pHitPoint = point;
            }
        }
    }
    return hit;
}

