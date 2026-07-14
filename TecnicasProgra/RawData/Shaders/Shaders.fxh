Texture2D NormalTexture : register(t0);
Texture2D ColorTexture : register(t1);
Texture2D SpecularTexture : register(t2);
SamplerState Sampler : register(s0);

struct PSIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PShader(PSIn input) : SV_Target0
{
    float3 normal = normalize(NormalTexture.Sample(Sampler, input.uv).rgb * 2.0f - 1.0f);
    float3 color = ColorTexture.Sample(Sampler, input.uv).rgb;
    float3 specularMap = SpecularTexture.Sample(Sampler, input.uv).rgb;

    float3 lightDirection = normalize(float3(-1, 1, -1));
    float3 ambient = color * 0.05f;

    float ndl = max(dot(normal, -lightDirection), 0.0f);
    float3 diffuse = ndl * color;

    // especular simple sin world position
    float3 viewDirection = normalize(float3(0, 0, 1));
    float3 halfVector = normalize(lightDirection + viewDirection);
    float specFactor = pow(max(dot(normal, halfVector), 0.0f), 16.0f);

    float3 specular = specFactor * specularMap;

    return float4(ambient + diffuse + specular, 1.0f);
}