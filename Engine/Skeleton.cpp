#include "stdafx.h"
#include "jsonUtil.h"
#include "Skeleton.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>


Matrix4 BoneTransform::ToMatrix() const
{
	Matrix4 ret = Matrix4::CreateFromQuaternion(mRot) * Matrix4::CreateTranslation(mPos);
	return ret;
}

/*static*/ BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
    BoneTransform bone;
    bone.mPos = Lerp(a.mPos, b.mPos, f);
    bone.mRot = Lerp(a.mRot, b.mRot, f);
    return bone;
}

/*static*/ Skeleton* Skeleton::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
    Skeleton* pSkel = new Skeleton();
    if (false == pSkel->Load(fileName))
    {
        delete pSkel;
        return nullptr;
    }
    return pSkel;
}

bool Skeleton::Load(const WCHAR* fileName)
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
        str != "itpskel" ||
        ver != 1)
    {
        return false;
    }

    const rapidjson::Value& bonecount = doc["bonecount"];
    if (!bonecount.IsUint())
    {
        return false;
    }

    size_t count = bonecount.GetUint();

    //    DbgAssert(count <= Skeleton::MAX_SKELETON_BONES, "Skeleton exceeds maximum allowed bones.");

    mBones.reserve(count);

    const rapidjson::Value& bones = doc["bones"];
    if (!bones.IsArray())
    {
        return false;
    }

    if (bones.Size() != count)
    {
        return false;
    }

    Bone temp;

    for (rapidjson::SizeType i = 0; i < count; i++)
    {
        if (!bones[i].IsObject())
        {
            return false;
        }

        const rapidjson::Value& name = bones[i]["name"];
        temp.mName = name.GetString();

        const rapidjson::Value& parent = bones[i]["parent"];
        temp.mParent = parent.GetInt();

        const rapidjson::Value& bindpose = bones[i]["bindpose"];
        if (!bindpose.IsObject())
            return false;
        if (false == GetQuaternionFromJSON(bindpose, "rot", temp.mBindPose.mRot))
            return false;
        if (false == GetVectorFromJSON(bindpose, "trans", temp.mBindPose.mPos))
            return false;

        mBones.push_back(temp);
    }

    // Now that we have the bones
    ComputeGlobalInvBindPose();

    return true;
}

void Skeleton::ComputeGlobalInvBindPose()
{
    mInvBind.resize(mBones.size());
    for (int i = 0; i < mBones.size(); ++i)
    {
        const Bone& bone = mBones[i];
        Matrix4 mat = bone.mBindPose.ToMatrix();
        if (bone.mParent >= 0)
            mInvBind[i] = mat * mInvBind[bone.mParent];
    }
    for (int i = 0; i < mBones.size(); ++i)
    {
        mInvBind[i].Invert();
    }
}