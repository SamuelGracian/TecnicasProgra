struct VSIn
{
    float4 position : POSITION0;
    float2 UV : TEXCOORD0;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 View;
    float4x4 Projection;
}

PSIn VShader(VSIn input)
{
    PSIn output;
    
    float4x4 viewProyection = mul(Projection , View);

    output.position = mul( viewProyection, input.position);
    
    output.UV = input.UV;

    return output;
}


float4 PShader(PSIn input) : SV_TARGET0
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
