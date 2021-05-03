#pragma once
#include "RenderObj.h"

class SkinnedObj : public RenderObj
{
public:
    enum { MAX_SKELETON_BONES = 80 };
    struct SkinConstants
    {
        Matrix4 c_skinMatrix[MAX_SKELETON_BONES];
    };

    SkinnedObj(Graphics* graphics, const Mesh* pMesh);
    virtual ~SkinnedObj();

    virtual void Update(float deltaTime);
    virtual void Draw();

    SkinConstants mSkinData;

private:
    ID3D11Buffer* mSkinBuffer;
};

