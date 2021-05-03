#include "stdafx.h"
#include "VertexBuffer.h"
#include "Graphics.h"


VertexBuffer::VertexBuffer(Graphics* graphics,
    const void* vertexData, uint32_t vertexCount, uint32_t vertexStride,
    const void* indexData, uint32_t indexCount, uint32_t indexStride
)
    : mGraphics(graphics)
    , mVertBuffer(nullptr)
    , mNumIndex(indexCount)
    , mVertStride(vertexStride)
{
    mVertBuffer = mGraphics->CreateGraphicsBuffer(vertexData, mVertStride * vertexCount, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
    mIndexBuffer = mGraphics->CreateGraphicsBuffer(indexData, indexStride * indexCount, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
    mIndexFormat = DXGI_FORMAT_R32_UINT;
    if (indexStride == 2)
    {
        mIndexFormat = DXGI_FORMAT_R16_UINT;
    }
}

VertexBuffer::~VertexBuffer()
{
    mVertBuffer->Release();
    mIndexBuffer->Release();
}

void VertexBuffer::Draw() const
{
    uint32_t strides[] = { mVertStride };
    uint32_t zeros[] = { 0 };
    mGraphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &mVertBuffer, strides, zeros);
    mGraphics->GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, mIndexFormat, 0);
    mGraphics->GetDeviceContext()->DrawIndexed(mNumIndex, 0, 0);
}