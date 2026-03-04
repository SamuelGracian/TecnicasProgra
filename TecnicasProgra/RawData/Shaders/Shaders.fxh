struct VSIn
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

PSIn VShader(VSIn input)
{
    PSIn output;

    output.position = input.position;
    output.color = input.color;

    return output;
}


float4 PShader(PSIn input) : SV_TARGET0
{
    return input.color;
}

cbuffer GAPIConstBuffer : register(b0)
{
    float a;
    float b;
    float c;
    float d;
}