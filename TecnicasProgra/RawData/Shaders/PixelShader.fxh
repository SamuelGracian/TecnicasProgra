Texture2D ColorTexture : register(t1);

SamplerState Sampler : register(s0);

Texture2D NormalTexture : register(t2);

Texture2D AlbedoTexture : register(t3);

Texture2D SpecularTexture : register(t4);

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
    //blinn phong
    float3 WorldPosition : TEXCOORD4;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 View;
    float4x4 Projection;
    matrix world;
    //blinn phong
    float3 CameraPosition;
    float Shininess;
}

PSIn VShader(VSIn input)
{
    PSIn output;
    
    float4x4 viewProyection = mul(mul(Projection, View), world);

    output.position = mul(viewProyection, input.position);
    
    output.WorldPosition = mul(world, input.position);
    
    output.UV = input.UV;
    
    float3 Normals = normalize(mul(world, float4(input.Normals.xyz, 0)).xyz);
    
    float3 Binormals = normalize(mul(world, float4(input.Binormal.xyz, 0)).xyz);
    
    float3 Tangents = normalize(mul(world, float4(input.Tangents.xyz, 0)).xyz);
    
    
    output.TBNmatrix = transpose(float3x3(Tangents, Binormals, Normals));
    
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
    
    //ambient light
    float3 Ambient = ((0.2156, 1, 1) * 0.05);
    
    //difuse 
    float NDL = max(dot(-lightDirection, normalize(Normal).xyz), 0.0);
    
    float3 Color = (AlbedoTexture.Sample(Sampler, input.UV).xyz);
    
    float3 Difuse = NDL * Color;
    
    
    //blinn phong
    float3 viewDirection = normalize(CameraPosition - input.WorldPosition);
    float3 halfVector = normalize(lightDirection + viewDirection);
    
    //specular
    
    float specFactor = pow(max(dot(Normal, halfVector), 0.0), Shininess);
    float3 specMap = SpecularTexture.Sample(Sampler, input.UV).rgb;

    float3 Specular = specFactor * specMap;
    
    float3 FinalColor = Difuse + Ambient + Specular;
    
    //return float4(Normal.xyz, 1);
    //return float4(FinalColor.xyz, 1);
    return float4(Difuse.xyz + Ambient.xyz + Specular.xyz, 1);
}