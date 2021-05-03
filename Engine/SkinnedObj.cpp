#include "stdafx.h"
#include "SkinnedObj.h"


SkinnedObj::SkinnedObj(Graphics* graphics, const Mesh* pMesh)
    : RenderObj(graphics, pMesh)
{
    for (int i = 0; i < MAX_SKELETON_BONES; ++i)
        mSkinData.c_skinMatrix[i] = Matrix4::Identity;
    mSkinBuffer = mGraphics->CreateGraphicsBuffer(&mSkinData, sizeof(mSkinData), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

SkinnedObj::~SkinnedObj()
{
    mSkinBuffer->Release();
}

/*virtual*/ void SkinnedObj::Update(float deltaTime)
{

    RenderObj::Update(deltaTime);
}

/*virtual*/ void SkinnedObj::Draw()
{
    mGraphics->UploadBuffer(mSkinBuffer, &mSkinData, sizeof(mSkinData));
    mGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_SKINNING, 1, &mSkinBuffer);
    RenderObj::Draw();
}
