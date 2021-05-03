#pragma once
#include "engineMath.h"
#include "Skeleton.h"
#include <vector>


class Animation
{
public:
	uint32_t GetNumBones() const { return mNumBones; }
	uint32_t GetNumFrames() const { return mNumFrames; }
	float GetLength() const { return mDuration; }
	static Animation* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	bool Load(const WCHAR* fileName);

	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const;

private:
	uint32_t mNumBones;
	uint32_t mNumFrames;
	float mDuration;
	std::vector<std::vector<BoneTransform>> mBones;
};

