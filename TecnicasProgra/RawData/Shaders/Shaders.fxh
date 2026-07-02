Texture2D ColorTexture : register(t1);

SamplerState Sampler : register(s0);

Texture2D NormalTexture : register(t2);

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
    float4 Normals : NORMAL0;
    float2 UV : TEXCOORD0;
    float3x3 TBNmatrix : TEXCOORD1;
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
    
    output.TBNmatrix = transpose(float3x3(input.Tangents.xyz, input.Binormal.xyz, input.Normals.xyz));
    
    return output;
}


float4 PShader(PSIn input) : SV_TARGET0
{
    //return float4(input.Normals.xyz, 1);
    //return NormalTexture.Sample(Sampler, input.UV);
    float3 Normal = normalize(NormalTexture.Sample(Sampler, input.UV).xyz);
    Normal = Normal * 2.0 - 1.0;
    Normal = mul(input.TBNmatrix, Normal);
    float3 lightDirection = normalize(float3(-1, 1, -1));
    //float NDL = dot(-lightDirection, normalize(NormalTexture.Sample(Sampler, input.UV).xyz));
    float NDL = dot(-lightDirection, normalize(Normal).xyz);
    //return float4(Normal.xyz, 1);
    return float4(NDL.xxx, 1);
}
