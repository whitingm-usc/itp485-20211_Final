#include "stdafx.h"
#include "Components\pointLight.h"
#include "jsonUtil.h"
#include "RenderObj.h"

PointLight::PointLight(Game* pGame, RenderObj* pObj)
	: Component(pObj)
	, mGame(pGame)
	, mLight(nullptr)
{
	mLight = mGame->AllocateLight();
}

PointLight::~PointLight()
{
	mGame->FreeLight(mLight);
}

/*virtual*/ void PointLight::LoadProperties(const rapidjson::Value& properties)
{
	GetVectorFromJSON(properties, "diffuseColor", mLight->diffuseColor);
	GetVectorFromJSON(properties, "specularColor", mLight->specularColor);
	GetFloatFromJSON(properties, "specularPower", mLight->specularPower);
	GetFloatFromJSON(properties, "innerRadius", mLight->innerRadius);
	GetFloatFromJSON(properties, "outerRadius", mLight->outerRadius);
}

/*virtual*/ void PointLight::Update(float deltaTime)
{
	mLight->position = mObj->mPerObjectConst.c_modelToWorld.GetTranslation();
}

