//**************************************************************//
//  Effect File exported by RenderMonkey 1.6
//
//  - Although many improvements were made to RenderMonkey FX  
//    file export, there are still situations that may cause   
//    compilation problems once the file is exported, such as  
//    occasional naming conflicts for methods, since FX format 
//    does not support any notions of name spaces. You need to 
//    try to create workspaces in such a way as to minimize    
//    potential naming conflicts on export.                    
//    
//  - Note that to minimize resulting name collisions in the FX 
//    file, RenderMonkey will mangle names for passes, shaders  
//    and function names as necessary to reduce name conflicts. 
//**************************************************************//

//--------------------------------------------------------------//
// waterShader
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string waterShader_Pass_0_Model : ModelData = "C:\\Program Files (x86)\\AMD\\RenderMonkey 1.82\\Examples\\Media\\Models\\OceanSurface.3ds";

struct VS_INPUT
{
    float4 mPosition : POSITION;
    float2 mUV : TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 mPosition : POSITION;
   float3 B : TEXCOORD0;
   float3 T : TEXCOORD1;
   float3 N : TEXCOORD2;
   float2 mUV : TEXCOORD3;
   float2 mUV1 : TEXCOORD4;
   float2 mUV2 : TEXCOORD5;
   float3 eyeVector : TEXCOORD7;
};

struct Wave
{
   float freq;
   float amp;
   float phase;
   float2 dir;
};

float4x4 WorldViewProjectionMatrix : WorldViewProjection;
float2 textureScale
<
   string UIName = "textureScale";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float2( 25.00, 25.00 );
float waveFreq
<
   string UIName = "waveFreq";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 0.03 );
float waveAmp
<
   string UIName = "waveAmp";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 1.80 );
float time : Time0_X;
float BumpScale
<
   string UIName = "BumpScale";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 1.00 );
float2 BumpSpeed
<
   string UIName = "BumpSpeed";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float2( 0.01, 0.00 );
float4 ViewPosition : ViewPosition;

VS_OUTPUT waterShader_Pass_0_Vertex_Shader_vs_main(VS_INPUT Input)
{
   VS_OUTPUT Output;
   
   #define NWAVES 2
   Wave wave[NWAVES] = {{1.0f,1.0f,0.5f,float2(-1,0)},{2.0,0.5,1.7,float2(-0.7,0.7)}};
      
   wave[0].freq = waveFreq;
   wave[0].amp = waveAmp;
   wave[1].freq = waveFreq*3.0f;
   wave[1].amp = waveAmp*0.33f;
   float4 P = Input.mPosition;
   
   //sum wave
   float ddx =0.0, ddy =0.0;
   float deriv;
   float angle;
    
   for(int i=0; i<NWAVES; ++i)
   {
      angle = dot(wave[i].dir,P.xz)*wave[i].freq+time*wave[i].phase;
      P.y +=wave[i].amp*sin(angle);
      deriv = wave[i].freq*wave[i].amp*cos(angle);
      ddx-=deriv*wave[i].dir.x;
      ddy-=deriv*wave[i].dir.y;
   }
   
   Output.B.xyz = BumpScale*normalize(float3(1,ddy,0)); //Binormal;
   Output.T.xyz = BumpScale*normalize(float3(1,ddx,0)); //Tangent;
   Output.N.xyz = BumpScale*normalize(float3(ddx,1,ddy)); //Normal;
   
   Output.mPosition = mul(P, WorldViewProjectionMatrix);
   Output.mUV = Input.mUV * textureScale+time*BumpSpeed;
   Output.mUV1 = Input.mUV * textureScale+time*BumpSpeed*2.0f; 
   Output.mUV2 = Input.mUV * textureScale+time*BumpSpeed*4.0f;   
   
   Output.eyeVector = P.xyz -ViewPosition;
   
   return Output;
}
struct PS_INPUT
{
   float3 B : TEXCOORD0;
   float3 T : TEXCOORD1;
   float3 N : TEXCOORD2;
   float2 mUV : TEXCOORD3;
   float2 mUV1 : TEXCOORD4;
   float2 mUV2 : TEXCOORD5;
   float3 eyeVector : TEXCOORD7;
};

texture NormalMap_Tex
<
   string ResourceName = "..\\..\\ได\?\Water Shader code\\waves2.dds";
>;
sampler2D NormalSampler = sampler_state
{
   Texture = (NormalMap_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};
texture EnvironmentMap_Tex
<
   string ResourceName = "..\\..\\ได\?\Water Shader code\\cubemap.dds";
>;
samplerCUBE EnvironmentSampler = sampler_state
{
   Texture = (EnvironmentMap_Tex);
};
float waterAmount
<
   string UIName = "waterAmount";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 0.30 );
float4 shallowColor
<
   string UIName = "shallowColor";
   string UIWidget = "Color";
   bool UIVisible =  true;
> = float4( 0.00, 1.00, 1.00, 1.00 );
float4 deepColor
<
   string UIName = "deepColor";
   string UIWidget = "Color";
   bool UIVisible =  true;
> = float4( 0.00, 0.30, 0.50, 1.00 );
float4 reflectionColor
<
   string UIName = "reflectionColor";
   string UIWidget = "Color";
   bool UIVisible =  true;
> = float4( 0.95, 1.00, 1.00, 1.00 );

float reflectionAmount
<
   string UIName = "reflectionAmount";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 1.00 );
float reflectionBlur
<
   string UIName = "reflectionBlur";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 0.00 );
float fresnelPower
<
   string UIName = "fresnelPower";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 5.00 );
float fresnelBias
<
   string UIName = "fresnelBias";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 0.33 );
float hdrMultiplier
<
   string UIName = "hdrMultiplier";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 0.47 );


float4 waterShader_Pass_0_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR0
{   
   float4 t0 = tex2D(NormalSampler,Input.mUV)*2.0f-1.0f;
   float4 t1 = tex2D(NormalSampler,Input.mUV1)*2.0f-1.0f;
   float4 t2 = tex2D(NormalSampler,Input.mUV2)*2.0f-1.0f;
   
   float3 N = normalize(t0.xyz+t1.xyz+t2.xyz);
   
   float3x3 m;
   m[0] = Input.B;
   m[1] = Input.T;
   m[2] = Input.N;
   
   N = normalize(mul(N,m));
   
   //reflection 
   float3 E = normalize(Input.eyeVector);
   float4 R;
   R.xyz = reflect(E,N);
   
   //Ogre conversion for cube map lookup
   R.z = -R.z;
   R.w = reflectionBlur;
   float4 reflection = texCUBEbias(EnvironmentSampler,R);
   //cheap hdr effect
   reflection.rgb*=(reflection.r+reflection.g+reflection.b)*hdrMultiplier;
   //fresnel
   float facing = 1.0 -max(dot(-E,N),0);
   float fresnel = saturate(fresnelBias + pow(facing, fresnelPower));
   
   float4 waterColor = lerp(shallowColor,deepColor,facing)*waterAmount;
   reflection = lerp(waterColor, reflection*reflectionColor,fresnel)*reflectionAmount;
   
   return waterColor+reflection;
}




//--------------------------------------------------------------//
// Technique Section for waterShader
//--------------------------------------------------------------//
technique waterShader
{
   pass Pass_0
   {
      VertexShader = compile vs_2_0 waterShader_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 waterShader_Pass_0_Pixel_Shader_ps_main();
   }

}

