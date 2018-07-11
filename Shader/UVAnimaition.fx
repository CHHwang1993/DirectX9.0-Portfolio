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
// UVAnimation
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string UVAnimation_Pass_0_Model : ModelData = "..\\..\\..\\..\\..\\..\\..\\..\\Program Files (x86)\\AMD\\RenderMonkey 1.82\\Examples\\Media\\Models\\Disc2.3ds";

struct VS_INPUT
{
   float4 mPosition : POSITION;
   float2 mUV : TEXCOORD0;
   float3 mNormal : NORMAL;
   float3 mTangent : TANGENT;
   float3 mBiNormal : BINORMAL;
};

struct VS_OUTPUT
{
   float4 mPosition : POSITION;
   float2 mUV : TEXCOORD0;
   float3 mViewDir : TEXCOORD1;
   float3 mLightDir : TEXCOORD2;
   float3 mNormal : TEXCOORD3;
   float3 mTangent : TEXCOORD4;
   float3 mBiNormal : TEXCOORD5;
};

float4x4 gWorldMatrix : World;
float4x4 gViewMatrix : View;
float4x4 gProjectionMatrix : Projection;
float4 gWorldLightPosition
<
   string UIName = "gWorldLightPosition";
   string UIWidget = "Direction";
   bool UIVisible =  false;
   float4 UIMin = float4( -10.00, -10.00, -10.00, -10.00 );
   float4 UIMax = float4( 10.00, 10.00, 10.00, 10.00 );
   bool Normalize =  false;
> = float4( 500.00, 500.00, -500.00, 1.00 );
float4 gWorldCameraPosition
<
   string UIName = "gWorldCameraPosition";
   string UIWidget = "Direction";
   bool UIVisible =  false;
   float4 UIMin = float4( -10.00, -10.00, -10.00, -10.00 );
   float4 UIMax = float4( 10.00, 10.00, 10.00, 10.00 );
   bool Normalize =  false;
> = float4( 0.00, 100.00, -200.00, 1.00 );

float gTime : Time0_X;
float gWaveHeight
<
   string UIName = "gWaveHeight";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 5.00 );
float gSpeed
<
   string UIName = "gSpeed";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 5.00 );
float gWaveFrequency
<
   string UIName = "gWaveFrequency";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 10.00 );
float gUVSpeed
<
   string UIName = "gUVSpeed";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 0.50 );

VS_OUTPUT UVAnimation_Pass_0_Vertex_Shader_vs_main(VS_INPUT Input)
{
   VS_OUTPUT Output;
   
   float cosTime = gWaveHeight*cos(gTime*gSpeed+Input.mUV.x*gWaveFrequency);
   
   Input.mPosition.y +=cosTime;
  
   
   float4 worldPosition = mul(Input.mPosition, gWorldMatrix);
   
   float3 lightDir = normalize(worldPosition.xyz - gWorldLightPosition.xyz);
   float3 viewDir = normalize(worldPosition.xyz - gWorldCameraPosition.xyz);
   
   Output.mLightDir = lightDir;
   Output.mViewDir = viewDir;
   
   float3 worldNormal = mul(Input.mNormal, (float3x3)gWorldMatrix);
   float3 worldTangent = mul(Input.mTangent, (float3x3)gWorldMatrix);
   float3 worldBinormal = mul(Input.mBiNormal, (float3x3)gWorldMatrix);
   
   Output.mNormal = normalize(worldNormal);
   Output.mBiNormal = normalize(worldBinormal);
   Output.mTangent = normalize(worldTangent);
   
   Output.mPosition = mul(worldPosition, gViewMatrix);
   Output.mPosition = mul(Output.mPosition, gProjectionMatrix);
  
   
   Output.mUV = Input.mUV+float2(gTime*gUVSpeed,gTime*gUVSpeed);
   
   return Output;
}
struct PS_INPUT
{
    float2 mUV : TEXCOORD0;
   float3 mLightDir : TEXCOORD1;
   float3 mViewDir : TEXCOORD2;
   float3 T : TEXCOORD3;
   float3 B: TEXCOORD4;
   float3 N : TEXCOORD5;
};

float3 gLightColor
<
   string UIName = "gLightColor";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float3( 0.70, 0.70, 1.00 );
float gParallaxIntensity
<
   string UIName = "gParallaxIntensity";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 0.50 );
texture DiffuseMap_Tex
<
   string ResourceName = "..\\Texture\\water.jpg";
>;
sampler2D DiffuseSampler = sampler_state
{
   Texture = (DiffuseMap_Tex);
};
texture NormalMap_Tex
<
   string ResourceName = "..\\Texture\\waterBump.jpg";
>;
sampler2D NormalSampler = sampler_state
{
   Texture = (NormalMap_Tex);
};

float4 UVAnimation_Pass_0_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR
{
   float h = tex2D(NormalSampler, Input.mUV).w;
   float2 E = normalize(-Input.mViewDir).xy;
   float2 Tex = Input.mUV +gParallaxIntensity*h*E;

   float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
    tangentNormal = normalize(tangentNormal*2-1);
    
    float3x3 TBN = float3x3(normalize(Input.T),normalize(Input.B),
     normalize(Input.N));
     
    TBN = transpose(TBN);
    
    float3 worldNormal = mul(TBN, tangentNormal);
    
    float3 lightDir = normalize(Input.mLightDir);
    float3 diffuse = saturate(dot(-lightDir, worldNormal));
    
    float4 albedo = tex2D(DiffuseSampler, Tex);
    
    diffuse = gLightColor * albedo.rgb * diffuse;
    
    float3 specular;
    
    if(diffuse.x >0)
    {   
       float3 reflection = reflect(lightDir, worldNormal);
       float3 viewDir = normalize(Input.mViewDir);
       
       specular = saturate(dot(-viewDir,reflection));
       specular = pow(specular, 25.0f);
      
       specular*=gLightColor;
    }
    
    float3 ambient = float3(0.1f,0.1f,0.1f)*albedo.rgb;
    
    return float4(ambient+diffuse+specular,1);
}
//--------------------------------------------------------------//
// Technique Section for UVAnimation
//--------------------------------------------------------------//
technique UVAnimation
{
   pass Pass_0
   {
      VertexShader = compile vs_2_0 UVAnimation_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 UVAnimation_Pass_0_Pixel_Shader_ps_main();
   }

}

