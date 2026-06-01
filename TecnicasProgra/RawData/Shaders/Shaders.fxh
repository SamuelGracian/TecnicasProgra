Texture2D ColorTexture : register(t1);

SamplerState Sampler : register(s0);

struct VSIn
{
    float4 position : POSITION0;
    float4 Normals : NORMAL0;
    float2 UV : TEXCOORD0;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float4 Normals : NORMAL0;
    float2 UV : TEXCOORD0;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 View;
    float4x4 Projection;
    matrix world;
}

PSIn VShader(VSIn input)
{
    PSIn output;
    
    
    float4x4 viewProyection = mul(mul(Projection, View), world);

    output.position = mul(viewProyection, input.position);
    
    output.UV = input.UV;

    output.Normals.xyz = normalize(input.Normals.xyz);
    
    return output;
}


float4 PShader(PSIn input) : SV_TARGET0
{
    return float4(input.Normals.xyz, 1);
    return ColorTexture.Sample(Sampler, input.UV);
}
