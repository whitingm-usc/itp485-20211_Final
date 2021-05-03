#pragma once
#include "component.h"
#include "game.h"
#include <string>
#include <unordered_map>

class Animation;
class Game;
class Skeleton;
class SkinnedObj;

class Character : public Component
{
public:
	Character(Game* pGame, SkinnedObj* pObj);
	~Character();

	virtual void LoadProperties(const rapidjson::Value& properties);
	virtual void Update(float deltaTime);

	bool SetAnim(const std::string& animName);
	void UpdateAnim(float deltaTime);
	void DoAnim();

protected:
	Game* mGame;
	SkinnedObj* mSkin;
	Skeleton* mSkel;
	std::unordered_map<std::string, const Animation*> mAnims;
	const Animation* mCurAnim;
	float mAnimTime;

	class AnimJob : public JobManager::Job
	{
	public:
		AnimJob(Character* pChar)
			: mChar(pChar)
		{}
		void DoIt() override
		{
			mChar->DoAnim();
		}

	private:
		Character* mChar;
	};
	AnimJob mAnimJob;
};