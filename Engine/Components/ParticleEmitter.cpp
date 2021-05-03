#include "stdafx.h"
#include "assetManager.h"
#include "ComputeShader.h"
#include "jsonUtil.h"
#include "ParticleEmitter.h"
#include "Profiler.h"
#include "RenderObj.h"
#include "stringUtil.h"
#include "texture.h"
#include "VertexBuffer.h"
#include "VertexFormat.h"
#include <algorithm>

/*static*/ std::vector<ParticleEmitter*> ParticleEmitter::s_allEmitters;

static Graphics* s_pGraphics = nullptr;
static AssetManager* s_pAssetManager = nullptr;
static ID3D11Buffer* s_pVertexBuffer = nullptr;
static ID3D11Buffer* s_pIndexBuffer = nullptr;

ParticleEmitter::ParticleEmitter(RenderObj* pObj)
    : Component(pObj)
    , mMatrix(Matrix4::Identity)
    , mFwdSpd(0.0f), mOutSpd(0.0f)
    , mEmitRate(0.0f)
    , mEmitDelay(0.0f)
    , mBlend(nullptr)
    , mTexture(nullptr)
    , mPartBuffer(nullptr)
    , mNumParticles(0)
{
    // Each emitter has a vertex buffer it uses to render all the particles
    // that belong to this emitter.
    Graphics* pGraphics = s_pAssetManager->GetGraphics();
    mPartBuffer = pGraphics->CreateGraphicsBuffer(&mPartRender, sizeof(mPartRender),
        D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

    // we keep a list of all emitters for RenderAll()
    s_allEmitters.push_back(this);
}

ParticleEmitter::~ParticleEmitter()
{
    // remove this emitter from the list of all emitters
    auto found = std::find(s_allEmitters.begin(), s_allEmitters.end(), this);
    if (s_allEmitters.end() != found)
    {
        auto last = s_allEmitters.end() - 1;
        std::iter_swap(found, last);
        s_allEmitters.pop_back();
    }

    // release the resources in use by this emitter
    mPartBuffer->Release();
    mBlend->Release();
}

// Load the properties
// This is used when you load a level
void ParticleEmitter::LoadProperties(const rapidjson::Value& properties)
{
    Vector3 pos = Vector3::Zero;
    GetVectorFromJSON(properties, "position", pos);
    Quaternion rot = Quaternion::Identity;
    GetQuaternionFromJSON(properties, "rotation", rot);
    mMatrix = Matrix4::CreateFromQuaternion(rot) * Matrix4::CreateTranslation(pos);
    {
        std::string textureName;
        if (GetStringFromJSON(properties, "texture", textureName))
        {
            std::wstring textureNameW;
            StringUtil::String2WString(textureNameW, textureName);
            mTexture = s_pAssetManager->LoadTexture(textureNameW);
        }
    }
    {
        std::string blendMode = "blend";
        GetStringFromJSON(properties, "blend", blendMode);
        if (blendMode == "add")
        {
            mBlend = s_pGraphics->CreateBlendState(true, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
        }
        else
        {
            mBlend = s_pGraphics->CreateBlendState(true, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
        }
    }
    GetFloatFromJSON(properties, "startSize", mConst.startSize);
    GetFloatFromJSON(properties, "endSize", mConst.endSize);
    GetFloatFromJSON(properties, "forwardSpeed", mFwdSpd);
    GetFloatFromJSON(properties, "outSpeed", mOutSpd);
    GetFloatFromJSON(properties, "gravity", mConst.gravity);
    GetColorFromJSON(properties, "startColor", mConst.startColor);
    GetColorFromJSON(properties, "endColor", mConst.endColor);
    GetFloatFromJSON(properties, "timeout", mConst.timeOut);
    GetFloatFromJSON(properties, "emissionRate", mEmitRate);
}

// Update the particle emitter.
// Called once per frame automatically by the RenderObj component update.
void ParticleEmitter::Update(float deltaTime)
{
    PROFILE_SCOPE(ParticleUpdate);

    // Emit new particles
    if (mEmitRate > 0.0f)
    {
        mEmitDelay += deltaTime;
        float emitThisFrame = mEmitRate * mEmitDelay;
        int numToEmit = (int)emitThisFrame;
        if (numToEmit > 0)
        {
            Emit(numToEmit);
            mEmitDelay -= numToEmit / mEmitRate;
        }
    }

#if 1   // TODO move this section into a compute shader
    // update all particles
    for (uint32_t i = 0; i < mNumParticles; ++i)
    {
        mPartUpdate[i].timer -= deltaTime;
        mPartUpdate[i].vel -= Vector3::UnitZ * mConst.gravity * deltaTime;
        Vector3 newPos = mPartRender[i].pos + mPartUpdate[i].vel * deltaTime;
        mPartRender[i].pos = newPos;
        float lerp = mPartUpdate[i].timer / mConst.timeOut;
        mPartRender[i].size = Math::Lerp(mConst.endSize, mConst.startSize, lerp);
        mPartRender[i].color.r = Math::Lerp(mConst.endColor.r, mConst.startColor.r, lerp);
        mPartRender[i].color.g = Math::Lerp(mConst.endColor.g, mConst.startColor.g, lerp);
        mPartRender[i].color.b = Math::Lerp(mConst.endColor.b, mConst.startColor.b, lerp);
        mPartRender[i].color.a = Math::Lerp(mConst.endColor.a, mConst.startColor.a, lerp);
    }
#endif

    // find expired particles and remove them.
    uint32_t firstParticle = mNumParticles;
    for (uint32_t i = 0; i < mNumParticles; ++i)
    {
        if (mPartUpdate[i].timer > 0.0f)
        {   // the oldest particles are first, so as soon as you find one that's still alive, you can delete everything up to this point
            firstParticle = i;
            break;
        }
    }
    if (firstParticle > 0)
    {   // delete the dead particles
        for (uint32_t i = firstParticle; i < mNumParticles; ++i)
        {
            mPartRender[i - firstParticle] = mPartRender[i];
            mPartUpdate[i - firstParticle] = mPartUpdate[i];
        }
        mNumParticles -= firstParticle;
    }
}

// Initialize the ParticleEmitter system.
// This should be called once and only once... not once per emitter - just once.
// Call this after pGraphics has been initialized, but before creating any emitters.
/*static*/ void ParticleEmitter::Init(Graphics* pGraphics, AssetManager* pAssetManager)
{
    static const VertexPosUV s_quad[] =
    {
        { Vector3(0.0f, -0.5f, -0.5f),	Vector2(0.0f, 1.0f) },
        { Vector3(0.0f,  0.5f, -0.5f),	Vector2(1.0f, 1.0f) },
        { Vector3(0.0f,  0.5f,  0.5f),	Vector2(1.0f, 0.0f) },
        { Vector3(0.0f, -0.5f,  0.5f),	Vector2(0.0f, 0.0f) }
    };
    static const uint16_t s_quadIndex[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    s_pGraphics = pGraphics;
    s_pAssetManager = pAssetManager;
    s_pVertexBuffer = s_pGraphics->CreateGraphicsBuffer(s_quad, sizeof(s_quad), D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
    s_pIndexBuffer = s_pGraphics->CreateGraphicsBuffer(s_quadIndex, sizeof(s_quadIndex), D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

// Shut down the ParticleEmitter system.
// This should be called once and only once... not once per emitter - just once.
/*static*/ void ParticleEmitter::Shutdown()
{
    while (s_allEmitters.size() > 0)
        delete s_allEmitters[0];
    s_pVertexBuffer->Release();
    s_pIndexBuffer->Release();
}

// Render all the ParticleEmitter.
// Call this once during your render.
/*static*/ void ParticleEmitter::RenderAll()
{
    s_pAssetManager->GetShader(L"Particle")->SetActive();
    for (auto& part : s_allEmitters)
    {
        part->Render();
    }
}

// Emit particles.
// You can specify how many particles to emit with "numToEmit".
void ParticleEmitter::Emit(uint32_t numToEmit)
{
    Matrix4 mat = mMatrix * mObj->mPerObjectConst.c_modelToWorld;
    while (numToEmit > 0 && mNumParticles < MAX_PARTICLES)
    {
        float ang = Math::TwoPi * (float)rand() / (float)RAND_MAX;
        float spd = mOutSpd * (float)rand() / (float)RAND_MAX;
        Vector3 vel(mFwdSpd, spd * cosf(ang), spd * sinf(ang));
        mPartRender[mNumParticles].pos = mat.GetTranslation();
        mPartRender[mNumParticles].size = mConst.startSize;
        mPartUpdate[mNumParticles].vel = Transform(vel, mat, 0.0f);
        mPartUpdate[mNumParticles].timer = mConst.timeOut;
        --numToEmit;
        ++mNumParticles;
    }
}


////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
// INTERNAL USE
////////////////////////////////////////////////////////////////////////////////

// Renders this ParticleEmitter.
// Called by RenderAll()
void ParticleEmitter::Render()
{
    if (nullptr == mTexture)
    {
        s_pGraphics->SetActiveTexture(Graphics::TEXTURE_SLOT_DIFFUSE, nullptr);
    }
    else
    {
        mTexture->SetActive(Graphics::TEXTURE_SLOT_DIFFUSE);
    }
    s_pGraphics->SetBlendState(mBlend);
    s_pGraphics->UploadBuffer(mPartBuffer, &mPartRender, sizeof(mPartRender));

    uint32_t strides[] = { sizeof(VertexPosUV), sizeof(PerParticleRenderData) };
    uint32_t offsets[] = { 0, 0 };
    ID3D11Buffer* bufferPointers[] = { s_pVertexBuffer, mPartBuffer };
    s_pGraphics->GetDeviceContext()->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
    s_pGraphics->GetDeviceContext()->IASetIndexBuffer(s_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    s_pGraphics->GetDeviceContext()->DrawIndexedInstanced(6, mNumParticles, 0, 0, 0);
}