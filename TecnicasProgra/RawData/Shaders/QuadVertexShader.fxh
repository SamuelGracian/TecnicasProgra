struct VSIn
{
    float2 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PSIn VShader(VSIn input)
{
    PSIn output;
    output.position = float4(input.position.xy, 0.0f, 1.0f);
    output.uv = input.uv;
    return output;
}