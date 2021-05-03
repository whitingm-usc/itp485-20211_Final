#include "stdafx.h"
#include "Components\character.h"
#include "Job.h"
#include "jsonUtil.h"
#include "Profiler.h"
#include "SkinnedObj.h"
#include "stringUtil.h"


Character::Character(Game* pGame, SkinnedObj* pObj)
	: Component(pObj)
	, mGame(pGame)
	, mSkin(pObj)
    , mSkel(nullptr)
    , mCurAnim(nullptr)
    , mAnimTime(0.0f)
    , mAnimJob(this)
{
}

Character::~Character()
{
}

/*virtual*/ void Character::LoadProperties(const rapidjson::Value& properties)
{
    Component::LoadProperties(properties);
    std::string skeleton;
    if (GetStringFromJSON(properties, "skeleton", skeleton))
    {
        std::wstring skeletonwstr;
        StringUtil::String2WString(skeletonwstr, skeleton);
        mSkel = mGame->GetAssetManager()->LoadSkeleton(skeletonwstr);
    }

    const rapidjson::Value& anims = properties["animations"];
    if (anims.IsArray())
    {
        for (rapidjson::SizeType i = 0; i < anims.Size(); ++i)
        {
            if (anims[i].IsArray() && anims[i].Size() == 2)
            {
                std::string animType = anims[i][0].GetString();
                std::string animName = anims[i][1].GetString();
                std::wstring animName_w;
                StringUtil::String2WString(animName_w, animName);
                mAnims[animType] = mGame->GetAssetManager()->LoadAnimation(animName_w);
            }
        }
    }
}

/*virtual*/ void Character::Update(float deltaTime)
{
    if (nullptr == mCurAnim)
        SetAnim("run");
    UpdateAnim(deltaTime);
}

bool Character::SetAnim(const std::string& animName)
{
    auto found = mAnims.find(animName);
    if (found != mAnims.end())
    {
        mCurAnim = found->second;
        mAnimTime = 0.0f;
//        mAnimTime = mCurAnim->GetLength() * rand() / RAND_MAX;
        return true;
    }
    return false;
}

void Character::UpdateAnim(float deltaTime)
{
    PROFILE_SCOPE(UpdateAnim);
    if (nullptr != mCurAnim)
    {
        mAnimTime += deltaTime;
        while (mAnimTime > mCurAnim->GetLength())
            mAnimTime -= mCurAnim->GetLength();
//        mGame->AddJob(&mAnimJob);
        DoAnim();
    }
}

void Character::DoAnim()
{
    std::vector<Matrix4> poses;
    mCurAnim->GetGlobalPoseAtTime(poses, mSkel, mAnimTime);
    for (int i = 0; i < mSkel->GetNumBones(); ++i)
        mSkin->mSkinData.c_skinMatrix[i] = mSkel->GetGlobalInvBindPoses()[i] * poses[i];
}

