#include "Constants.hlsl"

#define MAX_SKELETON_BONES 80
cbuffer SkinConstants : register(b3)
{
    float4x4 c_skinMatrix[MAX_SKELETON_BONES];
};

struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    uint4 boneIndex : BONE0;
    float4 boneWeight : WEIGHT0;
    float2 uv : TEXCOORD0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION1;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

VOut VS(VIn vIn)
{
    VOut output;

    float4 inPos = float4(vIn.position, 1.0f);
    float4 pos = mul(inPos, c_skinMatrix[vIn.boneIndex.x]) * vIn.boneWeight.x
        + mul(inPos, c_skinMatrix[vIn.boneIndex.y]) * vIn.boneWeight.y
        + mul(inPos, c_skinMatrix[vIn.boneIndex.z]) * vIn.boneWeight.z
        + mul(inPos, c_skinMatrix[vIn.boneIndex.w]) * vIn.boneWeight.w;
    float4 inNorm = float4(vIn.normal, 0.0f);
    float4 norm = mul(inNorm, c_skinMatrix[vIn.boneIndex.x]) * vIn.boneWeight.x
        + mul(inNorm, c_skinMatrix[vIn.boneIndex.y]) * vIn.boneWeight.y
        + mul(inNorm, c_skinMatrix[vIn.boneIndex.z]) * vIn.boneWeight.z
        + mul(inNorm, c_skinMatrix[vIn.boneIndex.w]) * vIn.boneWeight.w;

    output.worldPos = mul(pos, c_modelToWorld);
    output.position = mul(output.worldPos, c_viewProj);
    output.normal = mul(norm, c_modelToWorld);
    output.uv = vIn.uv;

    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float4 diffuseTex = DiffuseTexture.Sample(DefaultSampler, pIn.uv);

    // do the lighting
    float3 lightColor = c_ambient;
    float3 n = normalize(pIn.normal.xyz);
    float3 v = normalize(c_cameraPosition - pIn.worldPos);
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        if (c_pointLight[i].isEnabled)
        {
            float3 l = c_pointLight[i].position - pIn.worldPos.xyz;
            float dist = length(l);
            if (dist > 0.0)
            {
                l = l / dist;
                float falloff = smoothstep(c_pointLight[i].outerRadius, c_pointLight[i].innerRadius, dist);
                float3 d = falloff * c_pointLight[i].diffuseColor * max(0.0, dot(l, n));
                lightColor += d;

                float3 r = -reflect(l, n);
                float3 s = falloff * c_pointLight[i].specularColor * pow(max(0.0, dot(r, v)), c_pointLight[i].specularPower);
                lightColor += s;
            }
        }
    }

    // combine the final lighting with the vertex color and the texture color 
    float4 finalColor = diffuseTex * float4(lightColor, 1.0);
    return finalColor;
}
