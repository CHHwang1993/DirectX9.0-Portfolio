struct VS_INPUT
{
   float2 mPosition : POSITION;
   float2 mUV : TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 mPosition : POSITION;
   float4 mTexcoord : TEXCOORD0;
   float4 mDiffuse : COLOR0;
};

float4 posOffset;
float4 texOffset;
float4 FlareColor;

VS_OUTPUT vs_main(VS_INPUT Input)
{
  VS_OUTPUT Output;
  
  Output.mPosition.xy = (Input.mPosition.xy*posOffset.xy)+posOffset.zw;
  Output.mPosition.z = 0.5f;
  Output.mPosition.w = 1.0f;  

  Output.mTexcoord.xy = Input.mUV.xy + texOffset.zw;
  Output.mTexcoord.z = 0.0f;
  Output.mTexcoord.w = 1.0f;
  
  Output.mDiffuse = FlareColor;
  
  return Output;
}
struct PS_INPUT
{
   float4 mTexcoord : TEXCOORD0;
   float4 mDiffuse : COLOR0;
};

texture FlareMap_Tex;
sampler2D FlareSampler = sampler_state
{
   Texture = (FlareMap_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   ADDRESSW = WRAP;
};

float4 ps_main(PS_INPUT Input) : COLOR
{
   float4 color = tex2D(FlareSampler, Input.mTexcoord);
   color = color*Input.mDiffuse;
   
   return color;
}

technique LensFlare
{
   pass Pass_0
   {

      VertexShader = compile vs_2_0 vs_main();
      PixelShader = compile ps_2_0 ps_main();
   }

}

