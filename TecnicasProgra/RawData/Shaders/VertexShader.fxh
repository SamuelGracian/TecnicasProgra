Texture2D ColorTexture : register(t1);

SamplerState Sampler : register(s0);

Texture2D NormalTexture : register(t2);

Texture2D AlbedoTexture : register(t3);

Texture2D SpecularTexture : register(t4);

struct RenderTargets
{
    float4 Normals : SV_Target0;
    float4 Color : SV_Target1;
    float4 Specular : SV_Target2;
};

struct VSIn
{
    float4 position : POSITION0;
    float4 Normals : NORMAL0;
    float4 Tangents : TANGENT0;
    float4 Binormal : BINORMAL0;
    float2 UV : TEXCOORD0;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float2 UV : TEXCOORD0;
    float3x3 TBNmatrix : TEXCOORD1;
    // blinn phong
    float3 WorldPosition : TEXCOORD4;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 View;
    float4x4 Projection;
    matrix world;
    matrix ShadowView;
    matrix ShadowProyection;
    float3 CameraPosition;
    float Shininess;
}

PSIn VShader(VSIn input)
{
    PSIn output;

    float4x4 viewProjection = mul(mul(Projection, View), world);
    output.position = mul(viewProjection, input.position);
    
    output.WorldPosition = mul(world, input.position).xyz;
    output.UV = input.UV;

    float3 n = normalize(mul(world, float4(input.Normals.xyz, 0)).xyz);
    float3 b = normalize(mul(world, float4(input.Binormal.xyz, 0)).xyz);
    float3 t = normalize(mul(world, float4(input.Tangents.xyz, 0)).xyz);

    output.TBNmatrix = transpose(float3x3(t, b, n));
    return output;
}