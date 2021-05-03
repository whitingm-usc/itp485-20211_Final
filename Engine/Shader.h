#pragma once

class Graphics;

class Shader
{
public:
    Shader(Graphics* graphics);
    ~Shader();

    static bool LoadShader(const WCHAR* filename, const char* entryPoint, const char* model, ID3DBlob*& pBlobOut);
    bool Load(const WCHAR* fileName, const D3D11_INPUT_ELEMENT_DESC* layoutArray, int numLayoutElements);
    void SetActive() const;

private:
    Graphics* mGraphics;
    ID3D11VertexShader* mShaderVS;
    ID3D11PixelShader* mShaderPS;
    ID3D11InputLayout* mInputLayout;
};