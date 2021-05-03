#pragma once
#include "component.h"
#include "game.h"

class PointLight : public Component
{
public:
	PointLight(Game* pGame, RenderObj* pObj);
	~PointLight();

	virtual void LoadProperties(const rapidjson::Value& properties);
	virtual void Update(float deltaTime);

private:
	Game* mGame;
	Game::PointLightData* mLight;
};