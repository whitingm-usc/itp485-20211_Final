#pragma once
#include "engineMath.h"
#include "Graphics.h"

struct VertexPosColor
{
    Vector3 pos;
    Graphics::Color4 color;
};

struct VertexPosUV
{
    Vector3 pos;
    Vector2 uv;
};

struct VertexPosColorUV
{
    Vector3 pos;
    Graphics::Color4 color;
    Vector2 uv;
};

struct VertexPosNormUV
{
    Vector3 pos;
    Vector3 norm;
    Vector2 uv;
};

struct VertexPosNormColorUV
{
    Vector3 pos;
    Vector3 norm;
    Graphics::Color4 color;
    Vector2 uv;
};

struct VertexSkinned
{
    Vector3 pos;
    Vector3 norm;
    uint8_t boneIndex[4];
    uint8_t weight[4];
    Vector2 uv;
};

