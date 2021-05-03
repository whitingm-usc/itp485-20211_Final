#pragma once

class Graphics;

class VertexBuffer
{
public:
    VertexBuffer(Graphics* graphics,
        const void* vertexData, uint32_t vertexCount, uint32_t vertexStride,
        const void* indexData, uint32_t indexCount, uint32_t indexStride
    );

    ~VertexBuffer();

    void Draw() const;

private:
    Graphics* mGraphics;
    ID3D11Buffer* mVertBuffer;
    ID3D11Buffer* mIndexBuffer;
    uint32_t mNumIndex;
    uint32_t mVertStride;
    DXGI_FORMAT mIndexFormat;
};