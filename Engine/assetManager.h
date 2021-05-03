#pragma once
#include "Animation.h"
#include "assetCache.h"
#include "mesh.h"
#include "Shader.h"
#include "Skeleton.h"
#include "texture.h"

class Graphics;

class AssetManager
{
public:
    AssetManager(Graphics* pGraphics);
    ~AssetManager();

    void Clear();

    Graphics* GetGraphics() { return mGraphics; }

    Shader* GetShader(const std::wstring& shaderName);
    void SetShader(const std::wstring& shaderName, Shader* pShader);
    Texture* LoadTexture(const std::wstring& fileName);
    void SetMesh(const std::wstring& meshName, Mesh* pMesh);
    Mesh* LoadMesh(const std::wstring& fileName);
    Skeleton* LoadSkeleton(const std::wstring& fileName);
    Animation* LoadAnimation(const std::wstring& fileName);

private:
    Graphics* mGraphics;
    AssetCache<Shader> mShaderCache;
    AssetCache<Texture> mTextureCache;
    AssetCache<Mesh> mMeshCache;
    AssetCache<Skeleton> mSkeletonCache;
    AssetCache<Animation> mAnimCache;
};
