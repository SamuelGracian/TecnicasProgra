

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
    float depth : TEXCOORD0;
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

    float4x4 viewProjection = mul(mul(ShadowProyection, ShadowView), world);
    output.position = mul(viewProjection, float4(input.position.xyz, 1));
    return output;
}