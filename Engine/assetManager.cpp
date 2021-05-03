#include "stdafx.h"
#include "assetManager.h"
#include "Graphics.h"

AssetManager::AssetManager(Graphics* pGraphics)
    : mGraphics(pGraphics)
    , mShaderCache(this)
    , mTextureCache(this)
    , mMeshCache(this)
    , mSkeletonCache(this)
    , mAnimCache(this)
{
}

AssetManager::~AssetManager()
{
}

void AssetManager::Clear()
{
    mShaderCache.Clear();
    mTextureCache.Clear();
    mMeshCache.Clear();
    mSkeletonCache.Clear();
    mAnimCache.Clear();
}

Shader* AssetManager::GetShader(const std::wstring& shaderName)
{
    return mShaderCache.Get(shaderName);
}

void AssetManager::SetShader(const std::wstring& shaderName, Shader* pShader)
{
    mShaderCache.Cache(shaderName, pShader);
}

Texture* AssetManager::LoadTexture(const std::wstring& fileName)
{
    return mTextureCache.Load(fileName);
}

void AssetManager::SetMesh(const std::wstring& meshName, Mesh* pMesh)
{
    mMeshCache.Cache(meshName, pMesh);
}

Mesh* AssetManager::LoadMesh(const std::wstring& fileName)
{
    return mMeshCache.Load(fileName);
}

Skeleton* AssetManager::LoadSkeleton(const std::wstring& fileName)
{
    return mSkeletonCache.Load(fileName);
}

Animation* AssetManager::LoadAnimation(const std::wstring& fileName)
{
    return mAnimCache.Load(fileName);
}

