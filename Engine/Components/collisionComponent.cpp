#include "stdafx.h"
#include "Components\collisionComponent.h"
#include "jsonUtil.h"
#include "RenderObj.h"


CollisionComponent::CollisionComponent(Physics* pPhysics, RenderObj* pObj)
	: Component(pObj)
	, mPhysics(pPhysics)
{
	mPhysics->AddObj(this);
}

CollisionComponent::~CollisionComponent()
{
	mPhysics->RemoveObj(this);
}

/*virtual*/ void CollisionComponent::LoadProperties(const rapidjson::Value& properties)
{
    Component::LoadProperties(properties);
	GetVectorFromJSON(properties, "min", mAABB.mMin);
	GetVectorFromJSON(properties, "max", mAABB.mMax);
}

Physics::AABB CollisionComponent::GetAABB() const
{
	Physics::AABB aabb = mAABB;
	float scale = mObj->mPerObjectConst.c_modelToWorld.GetScale().x;
	Vector3 pos = mObj->mPerObjectConst.c_modelToWorld.GetTranslation();
	aabb.mMin *= scale;
	aabb.mMax *= scale;
	aabb.mMin += pos;
	aabb.mMax += pos;

	return aabb;
}
