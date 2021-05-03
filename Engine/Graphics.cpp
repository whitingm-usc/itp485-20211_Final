#include "stdafx.h"
#include "Graphics.h"
#include "Shader.h"
#include "engineMath.h"

#pragma comment (lib, "d3d11.lib") 

/*static*/ Graphics* Graphics::s_theGraphics = nullptr;


Graphics::Graphics()
    : mScreenWidth(0)
    , mScreenHeight(0)
    , mSwapChain(nullptr)
    , mDev(nullptr)
    , mDevCon(nullptr)
    , mBackBuffer(nullptr)
    , mDepthTex(nullptr)
    , mDepthView(nullptr)
    , mDepthState(nullptr)
    , mDepthAlphaState(nullptr)
    , mClampSampler(nullptr)
{
    DbgAssert(nullptr == s_theGraphics, "You can only have 1 Graphics");
    s_theGraphics = this;
}

Graphics::~Graphics()
{
    DbgAssert(this == s_theGraphics, "There can only be 1 Graphics");
    s_theGraphics = nullptr;
}

void Graphics::InitD3D(HWND hWnd, float width, float height)
{
    mScreenWidth = width;
    mScreenHeight = height;

    HRESULT hr = S_OK;

    {   // set up the swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = static_cast<UINT>(width);
        sd.BufferDesc.Height = static_cast<UINT>(height);
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
        UINT               numLevelsRequested = 1;
        D3D_FEATURE_LEVEL  FeatureLevelsSupported;
        hr = D3D11CreateDeviceAndSwapChain(NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
#ifdef _DEBUG
            D3D11_CREATE_DEVICE_DEBUG,
#else
            0,
#endif
            & FeatureLevelsRequested,
            numLevelsRequested,
            D3D11_SDK_VERSION,
            &sd,
            &mSwapChain,
            &mDev,
            &FeatureLevelsSupported,
            &mDevCon);
        DbgAssert(hr == S_OK, "Failed to create device");
    }

    {   // set up the viewport (full screen)
        SetViewport(0.0f, 0.0f, width, height);
    }

    {   // set CCW winding order
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = true;
        ID3D11RasterizerState* pRasterState;
        hr = mDev->CreateRasterizerState(&desc, &pRasterState);
        DbgAssert(hr == S_OK, "Something wrong with the rasterizer state");
        mDevCon->RSSetState(pRasterState);
        pRasterState->Release();
    }

    {   // grab the back-buffer as our render target
        ID3D11Texture2D* pBackBuffer;
        hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        DbgAssert(hr == S_OK, "Something wrong with your back buffer");
        hr = mDev->CreateRenderTargetView(pBackBuffer, nullptr, &mBackBuffer);
        DbgAssert(hr == S_OK, "Something went wrong with your back buffer");
        pBackBuffer->Release();
    }

    {   // we'll be drwaing triangle lists
        mDevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    {   // set up the z-buffer
        bool ret = CreateDepthStencil(static_cast<int>(width), static_cast<int>(height), &mDepthTex, &mDepthView);
        DbgAssert(ret, "Something went wrong with your depth buffer");
        mDepthState = CreateDepthStencilState(true, D3D11_COMPARISON_LESS_EQUAL);
        mDepthAlphaState = CreateDepthStencilState(true, D3D11_COMPARISON_LESS_EQUAL, false);
    }

    {   // texture sampler
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = mDev->CreateSamplerState(&sampDesc, &mClampSampler);
        SetActiveSampler(0, mClampSampler);
    }
}

void Graphics::BeginFrame(const Color4 &clearColor)
{
    mDevCon->OMSetRenderTargets(1, &mBackBuffer, mDepthView);
    mDevCon->OMSetDepthStencilState(mDepthState, 0);
    mDevCon->ClearDepthStencilView(mDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    mDevCon->ClearRenderTargetView(mBackBuffer, reinterpret_cast<const float*>(&clearColor));
}

void Graphics::EndFrame()
{
    mSwapChain->Present(1, 0);
}

void Graphics::CleanD3D()
{
#ifdef _DEBUG
    ID3D11Debug* pDbg = nullptr;
    HRESULT hr = mDev->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDbg));
    DbgAssert(S_OK == hr, "Unable to create debug device");
#endif

    mClampSampler->Release();

    mDepthState->Release();
    mDepthAlphaState->Release();
    mDepthView->Release();
    mDepthTex->Release();

    mSwapChain->Release();
    mDev->Release();
    mDevCon->Release();

    mBackBuffer->Release();

#ifdef _DEBUG
    pDbg->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
    pDbg->Release();
#endif
}

ID3D11Buffer* Graphics::CreateGraphicsBuffer(const void* initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage)
{
    ID3D11Buffer* buffer = nullptr;
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.Usage = inUsage;
    desc.ByteWidth = inDataSize;
    desc.BindFlags = inBindFlags;
    desc.CPUAccessFlags = inCPUAccessFlags;
    HRESULT hr = GetDevice()->CreateBuffer(&desc, nullptr, &buffer);
    DbgAssert(hr == S_OK, "Unable to create graphics buffer");

    if (nullptr != initialData)
        UploadBuffer(buffer, initialData, inDataSize);

    return buffer;
}

void Graphics::UploadBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize)
{
    D3D11_MAPPED_SUBRESOURCE map;
    HRESULT hr = GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
    DbgAssert(hr == S_OK, "Map failed");
    memcpy(map.pData, data, dataSize);
    GetDeviceContext()->Unmap(buffer, 0);
}

ID3D11Buffer* Graphics::CreateStructuredBuffer(const void* initialData, int inDataStride, int inDataCount, bool isInput)
{
    ID3D11Buffer* buffer = nullptr;
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    if (isInput)
    {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    }
    else
    {
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    }
    desc.ByteWidth = inDataStride * inDataCount;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = inDataStride;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = GetDevice()->CreateBuffer(&desc, nullptr, &buffer);
    DbgAssert(hr == S_OK, "Unable to create graphics buffer");

    if (isInput && nullptr != initialData)
        UploadBuffer(buffer, initialData, (size_t)inDataStride * (size_t)inDataCount);

    return buffer;
}

void Graphics::SetViewport(float x, float y, float width, float height)
{
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = x;
    viewport.TopLeftY = y;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    mDevCon->RSSetViewports(1, &viewport);
}

bool Graphics::CreateDepthStencil(int inWidth, int inHeight, ID3D11Texture2D** pDepthTexture, ID3D11DepthStencilView** pDepthView)
{
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = inWidth;
    descDepth.Height = inHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    HRESULT hr = mDev->CreateTexture2D(&descDepth, nullptr, pDepthTexture);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = mDev->CreateDepthStencilView(*pDepthTexture, &descDSV, pDepthView);
    return hr == 0;
}

ID3D11DepthStencilState* Graphics::CreateDepthStencilState(bool inDepthTestEnable, D3D11_COMPARISON_FUNC inDepthComparisonFunction, bool inDepthWriteEnable)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = inDepthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = inDepthComparisonFunction;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ID3D11DepthStencilState* depthState = nullptr;
    HRESULT hr = mDev->CreateDepthStencilState(&dsDesc, &depthState);

    return depthState;
}

void Graphics::SetActiveTexture(int slot, ID3D11ShaderResourceView* pView)
{
    mDevCon->PSSetShaderResources(slot, 1, &pView);
}

void Graphics::SetActiveSampler(int slot, ID3D11SamplerState* pSampler)
{
    mDevCon->PSSetSamplers(slot, 1, &pSampler);
}

ID3D11BlendState* Graphics::CreateBlendState(bool enable, D3D11_BLEND srcBlend, D3D11_BLEND dstBlend)
{
    D3D11_BLEND_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.RenderTarget[0].BlendEnable = enable;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].DestBlend = dstBlend;
    desc.RenderTarget[0].DestBlendAlpha = dstBlend;
    desc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
    desc.RenderTarget[0].SrcBlend = srcBlend;
    desc.RenderTarget[0].SrcBlendAlpha = srcBlend;
    ID3D11BlendState* blendState = nullptr;
    HRESULT hr = mDev->CreateBlendState(&desc, &blendState);
    DbgAssert(S_OK == hr, "Unable to create blend state");
    return blendState;
}

void Graphics::SetBlendState(ID3D11BlendState* inBlendState)
{
    float blendFactor[4];

    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;
    mDevCon->OMSetBlendState(inBlendState, blendFactor, 0xffffffff);
}

void Graphics::BeginAlpha()
{
    mDevCon->OMSetDepthStencilState(mDepthAlphaState, 0);
}

ID3D11ShaderResourceView* Graphics::CreateBufferSRV(ID3D11Buffer* pBuffer)
{
    D3D11_BUFFER_DESC descBuf = {};
    pBuffer->GetDesc(&descBuf);

    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    desc.BufferEx.FirstElement = 0;
    desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
    desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

    ID3D11ShaderResourceView* pSRV = nullptr;
    HRESULT hr = mDev->CreateShaderResourceView(pBuffer, &desc, &pSRV);
    DbgAssert(hr == S_OK, "CreateShaderResourceView failed");
    return pSRV;
}

ID3D11UnorderedAccessView* Graphics::CreateBufferUAV(ID3D11Buffer* pBuffer)
{
    D3D11_BUFFER_DESC descBuf = {};
    pBuffer->GetDesc(&descBuf);

    D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    desc.Buffer.FirstElement = 0;
    desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
    desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

    ID3D11UnorderedAccessView* pUAV = nullptr;
    HRESULT hr = mDev->CreateUnorderedAccessView(pBuffer, &desc, &pUAV);
    DbgAssert(hr == S_OK, "CreateUnorderedAccessView failed");
    return pUAV;
}
