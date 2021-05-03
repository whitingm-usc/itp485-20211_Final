#pragma once
#include "assetManager.h"
#include "engineMath.h"
#include "Graphics.h"
#include "Job.h"
#include "Physics.h"
#include <vector>

class Camera;
class RenderObj;
class Shader;
class Texture;

class Game
{
public:
	enum { MAX_POINT_LIGHTS = 8 };
	struct PointLightData
	{
		Vector3 diffuseColor;
		float pad1;

		Vector3 specularColor;
		float pad2;

		Vector3 position;
		float specularPower;

		float innerRadius;
		float outerRadius;
		bool isEnabled;
		float pad3;
	};

	struct LightingConstants
	{
		Vector3	c_ambient;
		float pad;
		PointLightData c_pointLight[MAX_POINT_LIGHTS];
	};

    Game();
    ~Game();

    void Init(HWND hWnd, float width, float height);
    void Shutdown();
	void Update(float deltaTime);
    void RenderFrame();

	void OnKeyDown(uint32_t key);
	void OnKeyUp(uint32_t key);
	bool IsKeyHeld(uint32_t key) const;
	
	AssetManager* GetAssetManager() { return &mAssetManager; }

	PointLightData* AllocateLight();
	void FreeLight(PointLightData* pLight);
	void SetAmbientLight(const Vector3& color);
	const Vector3& GetAmbientLight() const { return mLightData.c_ambient; }

	void AddJob(JobManager::Job* pJob) { mJobs.AddJob(pJob); }

	Camera* GetCamera() { return mCamera; }
	Physics* GetPhysics() { return &mPhysics; }

private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;

	Graphics mGraphics;
	AssetManager mAssetManager;
	JobManager mJobs;
	Physics mPhysics;

	std::vector<RenderObj*> mRenderObj;
	Camera* mCamera;

	LightingConstants mLightData;
	ID3D11Buffer* mLightConst;

	bool LoadLevel(const WCHAR* fileName);
};