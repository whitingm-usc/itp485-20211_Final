#pragma once
#include "RenderObj.h"

class AssetManager;
class Shader;
class Texture;

class RenderCube : public RenderObj
{
public:
    RenderCube(AssetManager* pAssetManager, const Shader* pShader, const Texture* pTex);
    ~RenderCube();
};

