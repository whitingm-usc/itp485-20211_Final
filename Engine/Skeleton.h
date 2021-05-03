#pragma once
#include "engineMath.h"
#include <string>
#include <vector>

class AssetManager;

class BoneTransform
{
public:
	Quaternion mRot;
	Vector3 mPos;
	Matrix4 ToMatrix() const;
	static BoneTransform Interpolate(
		const BoneTransform& a, const BoneTransform& b, float f);
};

class Skeleton
{
public:
	class Bone
	{
	public:
		BoneTransform mBindPose;
		std::string mName;
		int mParent;
	};

	size_t GetNumBones() const { return mBones.size(); }
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }
	const std::vector<Bone>& GetBones() const { return mBones; }
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mInvBind; }

	static Skeleton* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	bool Load(const WCHAR* fileName);

private:
	std::vector<Bone> mBones;
	std::vector<Matrix4> mInvBind;

	void ComputeGlobalInvBindPose();
};

