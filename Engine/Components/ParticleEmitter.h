#pragma once

#include "Component.h"
#include "engineMath.h"
#include "Graphics.h"
#include <vector>

class AssetManager;
class Texture;

class ParticleEmitter : public Component
{
public:
    enum { MAX_PARTICLES = 1024 };      // the maximum particles per emitter
    struct PerParticleRenderData        // the unique render data per particle
    {
        Vector3 pos;                    // center position of the particle in world space
        float size;                     // current width of the particle
        Graphics::Color4 color;         // current color of the particle
    };
    struct PerParticleUpdateData        // extra data needed for updating but not render
    {
        Vector3 vel;
        float timer;
    };
    struct ParticleConst                // these are the emitter constants
    {                                   // they do not vary per-particle
        float deltaTime = 0.0f;
        float gravity = 0.0f;
        float startSize = 0.0f;
        float endSize = 0.0f;
        Graphics::Color4 startColor;
        Graphics::Color4 endColor;
        float timeOut = 0.0f;
        float pad[3] = {};
    };

    ParticleEmitter(RenderObj* pObj);
    ~ParticleEmitter() override;
    void LoadProperties(const rapidjson::Value& properties) override;
    void Update(float deltaTime) override;
    static void Init(Graphics* pGraphics, AssetManager* pAssetManager);
    static void Shutdown();
    static void RenderAll();
    void Emit(uint32_t numToEmit = 1);

private:
    // Emitter Parameters
    Matrix4 mMatrix;            // The emitter can be placed on an offset from its host object
    ParticleConst mConst;       // The emitter constant parameters
    ID3D11BlendState* mBlend;   // the blend state (blend or add)
    const Texture* mTexture;    // the texture for this particle emitter
    float mFwdSpd, mOutSpd;     // original velocity for new particles
    float mEmitRate;            // how many particles to emit per second

    // internal stuff
    float mEmitDelay;   // how long until it's time for another particle
    static std::vector<ParticleEmitter*> s_allEmitters;

    // current particles
    PerParticleRenderData mPartRender[MAX_PARTICLES];
    PerParticleUpdateData mPartUpdate[MAX_PARTICLES];
    ID3D11Buffer* mPartBuffer;
    uint32_t mNumParticles;

    void Render();
};