#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 instancePos : POSITION1;
    float instanceSize : SIZE0;
    float4 instanceColor : COLOR0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

VOut VS(VIn vIn)
{
    VOut output;

    // We really should "billboard" these, but let's keep it simple.
    float3 pos = vIn.instanceSize * vIn.position + vIn.instancePos;
    output.position = float4(pos, 1.0);
    output.position = mul(output.position, c_viewProj);
    output.uv = vIn.uv;
    output.color = vIn.instanceColor;

    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float4 diffuseTex = DiffuseTexture.Sample(DefaultSampler, pIn.uv);
    diffuseTex *= pIn.color;
    return diffuseTex;
}

