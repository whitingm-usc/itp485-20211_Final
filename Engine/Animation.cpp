#include "stdafx.h"
#include "Animation.h"
#include "jsonUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>


/*static*/ Animation* Animation::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
    Animation* pAnim = new Animation();
    if (false == pAnim->Load(fileName))
    {
        delete pAnim;
        return nullptr;
    }
    return pAnim;
}

bool Animation::Load(const WCHAR* fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        return false;
    }

    std::stringstream fileStream;
    fileStream << file.rdbuf();
    std::string contents = fileStream.str();
    rapidjson::StringStream jsonStr(contents.c_str());
    rapidjson::Document doc;
    doc.ParseStream(jsonStr);

    if (!doc.IsObject())
    {
        return false;
    }

    std::string str = doc["metadata"]["type"].GetString();
    int ver = doc["metadata"]["version"].GetInt();

    // Check the metadata
    if (!doc["metadata"].IsObject() ||
        str != "itpanim" ||
        ver != 2)
    {
        return false;
    }

    const rapidjson::Value& sequence = doc["sequence"];
    if (!sequence.IsObject())
    {
        return false;
    }

    if (false == GetUintFromJSON(sequence, "frames", mNumFrames))
        return false;
    if (false == GetFloatFromJSON(sequence, "length", mDuration))
        return false;
    if (false == GetUintFromJSON(sequence, "bonecount", mNumBones))
        return false;

    mBones.resize(mNumBones);

    const rapidjson::Value& tracks = sequence["tracks"];
    if (!tracks.IsArray())
        return false;
    for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
    {
        if (!tracks[i].IsObject())
            return false;
        size_t boneIndex = tracks[i]["bone"].GetUint();

        const rapidjson::Value& transforms = tracks[i]["transforms"];
        if (!transforms.IsArray())
            return false;
        BoneTransform temp;
        if (transforms.Size() != mNumFrames)
            return false;

        for (rapidjson::SizeType j = 0; j < transforms.Size(); j++)
        {
            if (false == GetQuaternionFromJSON(transforms[j], "rot", temp.mRot))
                return false;
            if (false == GetVectorFromJSON(transforms[j], "trans", temp.mPos))
                return false;
            mBones[boneIndex].emplace_back(temp);
        }
    }

    return true;
}

void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const
{
    int fromKey = 0;
    int toKey = 0;
    float lerp = 0.0f;
    if (inTime > mDuration)
    {
        fromKey = mNumFrames - 1;
        toKey = fromKey;
    }
    else if (inTime > 0.0f)
    {
        float timePerFrame = mDuration / (mNumFrames - 1);
        float frac = inTime / timePerFrame;
        fromKey = (int)frac;
        toKey = fromKey + 1;
        lerp = frac - fromKey;
    }

    size_t numBones = inSkeleton->GetNumBones();
    outPoses.resize(numBones);
    for (size_t i = 0; i < numBones; ++i)
    {
        const Skeleton::Bone& bone = inSkeleton->GetBone(i);
        if (mBones[i].size() > 0)
        {
            BoneTransform xform = BoneTransform::Interpolate(mBones[i][fromKey], mBones[i][toKey], lerp);
            outPoses[i] = xform.ToMatrix();
        }
        else
            outPoses[i] = Matrix4::Identity;
        if (bone.mParent >= 0)
            outPoses[i] = outPoses[i] * outPoses[bone.mParent];
    }
}
