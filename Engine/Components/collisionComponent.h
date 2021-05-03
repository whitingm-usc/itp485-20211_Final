#pragma once
#include "component.h"
#include "Physics.h"


class CollisionComponent : public Component
{
public:
	CollisionComponent(Physics* pPhysics, RenderObj* pObj);
	~CollisionComponent();

	virtual void LoadProperties(const rapidjson::Value& properties);

	Physics::AABB GetAABB() const;

protected:
	Physics* mPhysics;
	Physics::AABB mAABB;
};