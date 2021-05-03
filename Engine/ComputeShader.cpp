#include "stdafx.h"
#include "ComputeShader.h"
#include "Graphics.h"
#include "Profiler.h"
#include "Shader.h"

// This just constructs an empty ComputeShader.
// Before you use it, you need to call Load() and add inputs and outputs with AddInput() and AddOutput().
// You may want some constant buffers with AddConst().
ComputeShader::ComputeShader(Graphics* pGraphics)
    : mGraphics(pGraphics)
    , mShaderCS(nullptr)
{
}

// Upon destruction, all the resources should be freed
ComputeShader::~ComputeShader()
{
    mShaderCS->Release();
    for (auto& ptr : mInBuf)
        ptr->Release();
    for (auto& ptr : mInView)
        ptr->Release();
    for (auto& ptr : mOutBuf)
        ptr->Release();
    for (auto& ptr : mOutView)
        ptr->Release();
    for (auto& ptr : mConstBuf)
        ptr->Release();
}

// Loads the ComputeShader
bool ComputeShader::Load(const WCHAR* fileName)
{
    ID3DBlob* pBlobCS = nullptr;
    if (Shader::LoadShader(fileName, "CS", "cs_4_0", pBlobCS))
    {
        ID3D11ComputeShader* computeShader = nullptr;
        HRESULT hr = mGraphics->GetDevice()->CreateComputeShader(pBlobCS->GetBufferPointer(), pBlobCS->GetBufferSize(), nullptr, &mShaderCS);
        DbgAssert(hr == S_OK, "Failed to create compute shader");
        if (nullptr != mShaderCS)
            return true;
        return false;
    }

    DbgAssert(false, "Failed to load shader");
    return false;
}

// Add an input buffer to the compute shader
// Specify these in the order you want the slots set up
void ComputeShader::AddInput(void* pData, uint32_t numElement, uint32_t stride)
{
    Data data = { pData, numElement, stride };
    mInData.push_back(data);
    ID3D11Buffer* inBuf = mGraphics->CreateStructuredBuffer(pData, stride, numElement, true);
    mInBuf.push_back(inBuf);
    ID3D11ShaderResourceView* inView = mGraphics->CreateBufferSRV(inBuf);
    mInView.push_back(inView);
}

// Update the data for an input buffer
// When you added the input with AddInput, the order you added them sets the "slot".
void ComputeShader::UpdateInput(uint32_t slot, void* pData, size_t numBytes)
{
    mGraphics->UploadBuffer(mInBuf[slot], pData, numBytes);
}

// Add an output buffer to the compute shader
// For cs model 4_0 (which we are using now), you may only have 1 output buffer per ComputeShader.
// If you upgrade to 5_0, you should be able to have more.
void ComputeShader::AddOutput(uint32_t numElement, uint32_t stride)
{
    Data data = { nullptr, numElement, stride };
    ID3D11Buffer* outBuf = mGraphics->CreateStructuredBuffer(nullptr, stride, numElement, false);
    mOutBuf.push_back(outBuf);
    ID3D11UnorderedAccessView* outView = mGraphics->CreateBufferUAV(outBuf);
    mOutView.push_back(outView);
}

// Add a constant buffer to the compute shader
void ComputeShader::AddConst(void* pData, uint32_t numBytes)
{
    ID3D11Buffer* constBuf = mGraphics->CreateGraphicsBuffer(pData, numBytes, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
    mConstBuf.push_back(constBuf);
}

// Update the data for an constant buffer
// When you added the constant buffer with AddConst, the order you added them sets the "slot".
void ComputeShader::UpdateConst(uint32_t slot, void* pData, size_t numBytes)
{
    mGraphics->UploadBuffer(mConstBuf[slot], pData, numBytes);
}

// Run the ComputeShader
// Specify the dimensions of your data with X, Y, and Z
// (For our example, we have a 1D data set with X=number of elements, Y and Z are 1.)
void ComputeShader::Run(uint32_t X, uint32_t Y, uint32_t Z)
{
    auto pCon = mGraphics->GetDeviceContext();

    // set the shader
    pCon->CSSetShader(mShaderCS, nullptr, 0);
    // set the input and output
    pCon->CSSetShaderResources(0, (uint32_t)mInView.size(), &mInView[0]);
    pCon->CSSetUnorderedAccessViews(0, (uint32_t)mOutView.size(), &mOutView[0], nullptr);
    if (false == mConstBuf.empty())
    {
        pCon->CSSetConstantBuffers(0, (uint32_t)mConstBuf.size(), &mConstBuf[0]);
    }
    // run the shader
    pCon->Dispatch(X, Y, Z);

    // disable everything when you're done
    pCon->CSSetShader(nullptr, nullptr, 0);
    ID3D11UnorderedAccessView* ppUAViewnullptr[1] = { nullptr };
    pCon->CSSetUnorderedAccessViews(0, 1, ppUAViewnullptr, nullptr);
    ID3D11ShaderResourceView* ppSRVnullptr[2] = { nullptr, nullptr };
    pCon->CSSetShaderResources(0, 2, ppSRVnullptr);
    ID3D11Buffer* ppCBnullptr[1] = { nullptr };
    pCon->CSSetConstantBuffers(0, 1, ppCBnullptr);
}

// Get the results from your ComputeShader.
// When you added the output buffer with AddOutput, the order you added them sets the "slot".
// (Remember CS model 4_0 only supports one output... slot 0.)
bool ComputeShader::GetResults(uint32_t slot, void* results, size_t numBytes)
{
    bool ret = false;
    ID3D11Buffer* outBuffer = nullptr;

    D3D11_BUFFER_DESC desc = {};
    mOutBuf[slot]->GetDesc(&desc);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    HRESULT hr = mGraphics->GetDevice()->CreateBuffer(&desc, nullptr, &outBuffer);
    if (S_OK == hr)
    {
        mGraphics->GetDeviceContext()->CopyResource(outBuffer, mOutBuf[slot]);
        D3D11_MAPPED_SUBRESOURCE map;
        {
            PROFILE_SCOPE(MapBuffer);
            hr = mGraphics->GetDeviceContext()->Map(outBuffer, 0, D3D11_MAP_READ, 0, &map);
        }
        if (S_OK == hr)
        {
            PROFILE_SCOPE(memcpy);
            memcpy(results, map.pData, numBytes);
            mGraphics->GetDeviceContext()->Unmap(outBuffer, 0);
            ret = true;
        }
        outBuffer->Release();
    }
    return ret;
}