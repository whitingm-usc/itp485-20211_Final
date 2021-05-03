#include "stdafx.h"
#include "Camera.h"
#include "Graphics.h"

Camera::Camera(Graphics* graphics)
	: mGraphics(graphics)
{
	mWorldToCamera = Matrix4::CreateTranslation(Vector3(0.0f, 0.0f, -100.f));
	mProjection = Matrix4::CreateRotationY(-Math::PiOver2)
		* Matrix4::CreateRotationZ(-Math::PiOver2)
		* Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
			mGraphics->GetScreenWidth(), mGraphics->GetScreenHeight(),
			25.0f, 10000.0f);
	mConstBuffer = mGraphics->CreateGraphicsBuffer(&mPerCameraConst, sizeof(mPerCameraConst),
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

Camera::~Camera()
{
	mConstBuffer->Release();
}

void Camera::SetActive()
{
	mPerCameraConst.c_viewProj = mWorldToCamera * mProjection;
	Matrix4 temp = mWorldToCamera;
	temp.Invert();
	mPerCameraConst.c_cameraPosition = temp.GetTranslation();
	mGraphics->UploadBuffer(mConstBuffer, &mPerCameraConst, sizeof(mPerCameraConst));
	mGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &mConstBuffer);
	mGraphics->GetDeviceContext()->PSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &mConstBuffer);
}