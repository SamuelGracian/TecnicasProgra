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

cbuffer GAPIConstBuffer : register(b0)
{
    float cosValue;
    float amplitude;
    float c;
    float d;
}

PSIn VShader(VSIn input)
{
    PSIn output;
    
    output.position = input.position;
    output.position.x += (cosValue * amplitude);
    //output.position = input.position;
    output.color = input.color;

    return output;
}


float4 PShader(PSIn input) : SV_TARGET0
{
    return input.color;
}
