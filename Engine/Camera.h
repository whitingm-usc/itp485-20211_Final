#pragma once
#include "engineMath.h"

class Graphics;

class Camera
{
public:
	struct PerCameraConstants
	{
		Matrix4 c_viewProj;
		Vector3 c_cameraPosition;
		float pad;
	};

	Camera(Graphics* graphics);
	~Camera();

	void SetActive();

	Matrix4 mWorldToCamera;
	Matrix4 mProjection;

private:
	Graphics* mGraphics;
	PerCameraConstants mPerCameraConst;
	ID3D11Buffer* mConstBuffer;
};

