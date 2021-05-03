#pragma once
#include <vector>

class Graphics;

////////////////////////////////////////////////////////////////////////////////
// ComputeShader
//
// A helper class to make it easy to add a compute shader to your project.
//
// //Set up your ComputeShader like so:
// MyInputStruct myInput[NUM_ELEMENT];
// MyOutputStruct myOutput[NUM_ELEMENT];
// MyConstStruct myConst;
// ComputeShader* myCS = new ComputeShader(pGraphics);
// myCS->Load(L"MyShader.hlsl");
// myCS->AddInput(&myInput, NUM_ELEMENT, sizeof(MyInputStruct));
// myCS->AddOutput(NUM_ELEMENT, sizeof(MyOutputStruct));
// myCS->AddConst(&myConst, sizeof(myConst));
//
// //Now that it's set up, you can run it with:
// myCS->Run(NUM_ELEMENT, 1, 1);
//
// //And you get your answer with:
// myCS->GetResults(0, &myOutput, sizeof(myOutput));
//
// //Between runs, your data could change. Update your inputs and constants like this:
// myCS->UpdateInput(0, &myInput, sizeof(myInput));
// myCS->UpdateConst(0, &myConst, sizeof(myConst));
//
// //When you're all done, just delete it to clean up:
// delete myCS;
////////////////////////////////////////////////////////////////////////////////
class ComputeShader
{
public:
    ComputeShader(Graphics* graphics);
    ~ComputeShader();

    bool Load(const WCHAR* fileName);
    void AddInput(void* pData, uint32_t numElement, uint32_t stride);
    void UpdateInput(uint32_t slot, void* pData, size_t numBytes);
    void AddOutput(uint32_t numElement, uint32_t stride);
    void AddConst(void* pData, uint32_t numBytes);
    void UpdateConst(uint32_t slot, void* pData, size_t numBytes);

    void Run(uint32_t X, uint32_t Y = 1, uint32_t Z = 1);
    bool GetResults(uint32_t slot, void* results, size_t numBytes);

private:
    struct Data
    {
        void* pData;
        uint32_t numElement;
        uint32_t stride;
    };
    Graphics* mGraphics;
    ID3D11ComputeShader* mShaderCS;
    std::vector<Data> mInData;
    std::vector<ID3D11Buffer*> mInBuf;
    std::vector<ID3D11ShaderResourceView*> mInView;
    std::vector<ID3D11Buffer*> mOutBuf;
    std::vector<ID3D11UnorderedAccessView*> mOutView;
    std::vector<ID3D11Buffer*> mConstBuf;
};