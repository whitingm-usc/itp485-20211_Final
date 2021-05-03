#include "stdafx.h"
#include "Component.h"
#include "mesh.h"
#include "RenderObj.h"
#include "Shader.h"
#include "texture.h"
#include "VertexBuffer.h"


RenderObj::RenderObj(Graphics* graphics, const Mesh* pMesh)
    : mGraphics(graphics)
    , mConstBuffer(nullptr)
    , mMesh(pMesh)
{
    mPerObjectConst.c_modelToWorld = Matrix4::CreateRotationZ(Math::ToRadians(45.0f));
    mConstBuffer = mGraphics->CreateGraphicsBuffer(&mPerObjectConst, sizeof(mPerObjectConst), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

RenderObj::~RenderObj()
{
    mConstBuffer->Release();
}

/*virtual*/ void RenderObj::Update(float deltaTime)
{
    for (Component* pComp : mComponents)
        pComp->Update(deltaTime);
}

/*virtual*/ void RenderObj::Draw()
{
    if (nullptr != mMesh)
    {
        mGraphics->UploadBuffer(mConstBuffer, &mPerObjectConst, sizeof(mPerObjectConst));
        mGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_RENDEROBJ, 1, &mConstBuffer);
        mMesh->Draw();
    }
}
