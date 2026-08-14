Texture2D NormalTexture : register(t2);
Texture2D AlbedoTexture : register(t3);
Texture2D SpecularTexture : register(t4);
Texture2D ShadowMap : register(t5);
SamplerState Sampler : register(s0);

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
    float3 WorldPosition : TEXCOORD4;
    float4 ShadowPosition : TEXCOORD5;
};

struct RenderTargets
{
    float4 Normals : SV_Target0;
    float4 Color : SV_Target1;
    float4 Specular : SV_Target2;
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
    
    float4 worldPosition = mul(world, input.position);

    output.position = mul(viewProjection, input.position);

    output.WorldPosition = worldPosition.xyz;

    output.ShadowPosition = mul(mul(ShadowProyection, ShadowView), worldPosition);
    
    output.position = mul(viewProjection, float4(input.position.xyz, 1));
    
    output.UV = input.UV;
    
    output.WorldPosition = mul(world, input.position).xyz;
    
    float3 n = normalize(mul(world, float4(input.Normals.xyz, 0)).xyz);
    
    float3 b = normalize(mul(world, float4(input.Binormal.xyz, 0)).xyz);
    
    float3 t = normalize(mul(world, float4(input.Tangents.xyz, 0)).xyz);
    
    output.TBNmatrix = transpose(float3x3(t, b, n));
    
    return output;
}

RenderTargets PShader(PSIn input)
{
    RenderTargets output;

    float3 normal = normalize(NormalTexture.Sample(Sampler, input.UV).xyz * 2.0f - 1.0f);
    normal = normalize(mul(input.TBNmatrix, normal));

    float3 color = AlbedoTexture.Sample(Sampler, input.UV).rgb;
    float3 spec = SpecularTexture.Sample(Sampler, input.UV).rgb;
    
  // light
    float3 lightDirection = normalize(float3(-1, 1, -1));
    float3 ambient = color * 0.05f;

    float ndl = max(dot(normal, -lightDirection), 0.0f);
    float3 diffuse = ndl * color;

    float3 viewDirection = normalize(float3(0, 0, 1));
    float3 halfVector = normalize(lightDirection + viewDirection);
    float specFactor = pow(max(dot(normal, halfVector), 0.0f), 16.0f);
    float3 specular = specFactor * spec;

    //float4 lightSpacePos = mul(mul(ShadowProyection, ShadowView), float4(input.WorldPosition, 1.0f));
    float4 lightSpacePos = input.ShadowPosition;

    //float inverseW = rcp(lightSpacePos.w);

    //float2 shadowUV = lightSpacePos.xy * inverseW * 0.5f + 0.5f;

    //float currentDepth =lightSpacePos.z * inverseW * 0.5f + 0.5f;

    float bias = max(0.005f * (1.0f - dot(normal, -lightDirection)), 0.0005f);


    float shadow = 1.0f;

   
    if (abs(lightSpacePos.w) > 0.00001f)
    {
        float3 normalizedLightPosition = lightSpacePos.xyz / lightSpacePos.w;

        float2 shadowUV;

        shadowUV.x = normalizedLightPosition.x * 0.5f + 0.5f;

        shadowUV.y = -normalizedLightPosition.y * 0.5f + 0.5f;

        float currentDepth = normalizedLightPosition.z;

        if (shadowUV.x >= 0.0f && shadowUV.x <= 1.0f && shadowUV.y >= 0.0f && shadowUV.y <= 1.0f && currentDepth >= 0.0f && currentDepth <= 1.0f)
        {
            float shadowDepth = ShadowMap.Sample(Sampler, shadowUV).r;

            float bias = 0.005f;

            shadow = currentDepth - bias > shadowDepth ? 0.0f : 1.0f;
        }
    }

    diffuse *= shadow;
    specular *= shadow;

    output.Normals = float4(normal * 0.5f + 0.5f, 1.0f);
    output.Color = float4(color, shadow);
    output.Specular = float4(spec, 1.0f);

    return output;
}