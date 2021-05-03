#include "stdafx.h"
#include "assetManager.h"
#include "mesh.h"
#include "RenderCube.h"
#include "VertexBuffer.h"
#include "VertexFormat.h"


RenderCube::RenderCube(AssetManager* pAssetManager, const Shader* pShader, const Texture* pTex)
	: RenderObj(pAssetManager->GetGraphics(), nullptr)
{
	static VertexPosNormColorUV cubeVertex[] =
	{
		// Red Face
		{ Vector3(-0.5f,  0.5f, -0.5f),	Vector3( 0.0f,  0.0f, -1.0f),	Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f),	Vector2(0.0f, 0.0f) },
		{ Vector3( 0.5f,  0.5f, -0.5f), Vector3( 0.0f,  0.0f, -1.0f),	Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f),	Vector2(1.0f, 0.0f) },
		{ Vector3( 0.5f, -0.5f, -0.5f), Vector3( 0.0f,  0.0f, -1.0f),	Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f),	Vector2(1.0f, 1.0f) },
		{ Vector3(-0.5f, -0.5f, -0.5f), Vector3( 0.0f,  0.0f, -1.0f),	Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f),	Vector2(0.0f, 1.0f) },
		// Green Face								    
		{ Vector3(-0.5f,  0.5f,  0.5f),	Vector3( 0.0f,  0.0f,  1.0f),	Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f),	Vector2(0.0f, 0.0f) },
		{ Vector3( 0.5f,  0.5f,  0.5f),	Vector3( 0.0f,  0.0f,  1.0f),	Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f),	Vector2(1.0f, 0.0f) },
		{ Vector3( 0.5f, -0.5f,  0.5f),	Vector3( 0.0f,  0.0f,  1.0f),	Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f),	Vector2(1.0f, 1.0f) },
		{ Vector3(-0.5f, -0.5f,  0.5f),	Vector3( 0.0f,  0.0f,  1.0f),	Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f),	Vector2(0.0f, 1.0f) },
		// Blue Face
		{ Vector3(-0.5f, -0.5f,  0.5f),	Vector3(-1.0f,  0.0f,  0.0f),	Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f),	Vector2(0.0f, 0.0f) },
		{ Vector3(-0.5f,  0.5f,  0.5f), Vector3(-1.0f,  0.0f,  0.0f),	Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f),	Vector2(1.0f, 0.0f) },
		{ Vector3(-0.5f,  0.5f, -0.5f), Vector3(-1.0f,  0.0f,  0.0f),	Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f),	Vector2(1.0f, 1.0f) },
		{ Vector3(-0.5f, -0.5f, -0.5f), Vector3(-1.0f,  0.0f,  0.0f),	Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f),	Vector2(0.0f, 1.0f) },
		// Yellow Face
		{ Vector3( 0.5f, -0.5f,  0.5f),	Vector3( 1.0f,  0.0f,  0.0f),	Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f),	Vector2(0.0f, 0.0f) },
		{ Vector3( 0.5f,  0.5f,  0.5f), Vector3( 1.0f,  0.0f,  0.0f),	Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f),	Vector2(1.0f, 0.0f) },
		{ Vector3( 0.5f,  0.5f, -0.5f), Vector3( 1.0f,  0.0f,  0.0f),	Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f),	Vector2(1.0f, 1.0f) },
		{ Vector3( 0.5f, -0.5f, -0.5f), Vector3( 1.0f,  0.0f,  0.0f),	Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f),	Vector2(0.0f, 1.0f) },
		// Cyan Face
		{ Vector3( 0.5f, -0.5f, -0.5f),	Vector3( 0.0f, -1.0f,  0.0f),	Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f),	Vector2(0.0f, 0.0f) },
		{ Vector3( 0.5f, -0.5f,  0.5f), Vector3( 0.0f, -1.0f,  0.0f),	Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f),	Vector2(1.0f, 0.0f) },
		{ Vector3(-0.5f, -0.5f,  0.5f), Vector3( 0.0f, -1.0f,  0.0f),	Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f),	Vector2(1.0f, 1.0f) },
		{ Vector3(-0.5f, -0.5f, -0.5f), Vector3( 0.0f, -1.0f,  0.0f),	Graphics::Color4(0.0f, 1.0f, 1.0f, 1.0f),	Vector2(0.0f, 1.0f) },
		// Purple Face
		{ Vector3( 0.5f,  0.5f, -0.5f),	Vector3( 0.0f,  1.0f,  0.0f),	Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f),	Vector2(0.0f, 0.0f) },
		{ Vector3( 0.5f,  0.5f,  0.5f), Vector3( 0.0f,  1.0f,  0.0f),	Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f),	Vector2(1.0f, 0.0f) },
		{ Vector3(-0.5f,  0.5f,  0.5f), Vector3( 0.0f,  1.0f,  0.0f),	Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f),	Vector2(1.0f, 1.0f) },
		{ Vector3(-0.5f,  0.5f, -0.5f), Vector3( 0.0f,  1.0f,  0.0f),	Graphics::Color4(1.0f, 0.0f, 1.0f, 1.0f),	Vector2(0.0f, 1.0f) },
	};
	static uint16_t cubeIndex[] =
	{
		// Red Face
		2, 1, 0,
		3, 2, 0,
		// Green Face
		4, 5, 6,
		4, 6, 7,
		// Blue Face
		10, 9, 8,
		11, 10, 8,
		// Yellow Face
		12, 13, 14,
		12, 14, 15,
		// Cyan Face
		18, 17, 16,
		19, 18, 16,
		// Purple Face
		20, 21, 22,
		20, 22, 23,
	};
	VertexBuffer* pVertBuffer = new VertexBuffer(mGraphics,
		cubeVertex, ARRAY_SIZE(cubeVertex), sizeof(cubeVertex[0]),
		cubeIndex, ARRAY_SIZE(cubeIndex), sizeof(cubeIndex[0])
		);
	Mesh* pMesh = new Mesh(pAssetManager, pVertBuffer, pShader);
	pMesh->SetTexture(Graphics::TEXTURE_SLOT_DIFFUSE, pTex);
	mMesh = pMesh;
}

RenderCube::~RenderCube()
{
	delete mMesh;
}
