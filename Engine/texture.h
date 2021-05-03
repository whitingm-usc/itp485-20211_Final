#pragma once

class AssetManager;
class Graphics;

class Texture
{
public:
    Texture(Graphics *graphics);
    ~Texture();
    
    void Free();
    bool Load(const WCHAR* fileName);
    static Texture* StaticLoad(const WCHAR* fileName, AssetManager* pManager);
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
    void SetActive(int slot) const;

private:
    Graphics *mGraphics;
    ID3D11Resource *mResource;
    ID3D11ShaderResourceView *mView;
    int mWidth;
    int mHeight;
};