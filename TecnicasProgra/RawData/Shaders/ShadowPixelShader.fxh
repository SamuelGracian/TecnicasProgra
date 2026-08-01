

struct PSIn
{
    float4 position : SV_POSITION;
    float depth : TEXCOORD0;
};

struct RenderTargets
{
    float4 ShadowDepth : SV_Target0;
};

RenderTargets PShader(PSIn input)
{       
    RenderTargets output;
    output.ShadowDepth = input.depth.xxxx;

    return output;
   
}