#pragma once
#include "engineMath.h"
#include "Graphics.h"

class Component;
class Mesh;
class Shader;

class RenderObj
{
public:
    struct PerObjectConstants
    {
        Matrix4 c_modelToWorld;
    };

    RenderObj(Graphics* graphics, const Mesh* pMesh);
    virtual ~RenderObj();
    void AddComponent(Component* pComp) { mComponents.emplace_back(pComp); }

    virtual void Update(float deltaTime);
    virtual void Draw();

    PerObjectConstants mPerObjectConst;

protected:
    Graphics* mGraphics;
    ID3D11Buffer* mConstBuffer;
    const Mesh* mMesh;
    std::vector<Component*> mComponents;
};

