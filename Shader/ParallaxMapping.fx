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
// NormalMapping
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string NormalMapping_Pass_0_Model : ModelData = "..\\..\\..\\..\\..\\..\\..\\..\\Program Files (x86)\\AMD\\RenderMonkey 1.82\\Examples\\Media\\Models\\Teapot.3ds";

struct VS_INPUT
{
   float4 mPosition : POSITION;
   float3 mNormal : NORMAL;
   float3 mTangent : TANGENT;
   float3 mBinormal : BINORMAL;
   float2 mUV : TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 mPosition : POSITION;
   float2 mUV : TEXCOORD0;
   float3 mLightDir : TEXCOORD1;
   float3 mViewDir : TEXCOORD2;
   float3 T : TEXCOORD3;
   float3 B : TEXCOORD4;
   float3 N : TEXCOORD5;
};

float4x4 gWorldViewProjectionMatrix : WorldViewProjection;
float4x4 gWorldMatrix : World;
float4x4 LightMatrix;
float4 gWorldCameraPosition : ViewPosition;

VS_OUTPUT NormalMapping_Pass_0_Vertex_Shader_vs_main(VS_INPUT Input)
{
   VS_OUTPUT Output;
   
   Output.mPosition = mul(Input.mPosition, gWorldViewProjectionMatrix);
   
   Output.mUV = Input.mUV;
   
   float4 worldPosition = mul(Input.mPosition, gWorldMatrix);
   
   float3 lightDir = float3(LightMatrix._21, LightMatrix._22,LightMatrix._23);
   float3 viewDir = normalize(worldPosition.xyz - gWorldCameraPosition.xyz);
   
   Output.mLightDir = lightDir;
   Output.mViewDir = viewDir;
   
   float3 worldNormal = mul(Input.mNormal, (float3x3)gWorldMatrix);
   float3 worldTangent = mul(Input.mTangent, (float3x3)gWorldMatrix);
   float3 worldBinormal = mul(Input.mBinormal, (float3x3)gWorldMatrix);
   
   Output.N = normalize(worldNormal);
   Output.B = normalize(worldBinormal);
   Output.T = normalize(worldTangent);
   
   
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
> = float( 0.07 );
texture DiffuseMap_Tex
<
   string ResourceName = "E:\\DownLoad\\3DGame2ndEd\\Part5\\Chap13-Shaders\\02-NormalMapping\\[13-02]parallax\\relief_rockwall.jpg";
>;
sampler2D DiffuseSampler = sampler_state
{
   Texture = (DiffuseMap_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   MIPFILTER = LINEAR;
};
texture NormalMap_Tex
<
   string ResourceName = "E:\\DownLoad\\3DGame2ndEd\\Part5\\Chap13-Shaders\\02-NormalMapping\\[13-02]parallax\\relief_rockwall.tga";
>;
sampler2D NormalSampler = sampler_state
{
   Texture = (NormalMap_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   MIPFILTER = LINEAR;
};
texture SpecularMap_Tex
<
   string ResourceName = "E:\\ได\?\05_DiffuseSpecularMapping\\fieldstone_SM.tga";
>;
sampler2D SpecularSampler = sampler_state
{
   Texture = (SpecularMap_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   MIPFILTER = LINEAR;
};

float4 NormalMapping_Pass_0_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR
{
   float h = tex2D(NormalSampler, Input.mUV).w;
   float E = normalize(-Input.mViewDir).xy;
   float2 Tex = Input.mUV + gParallaxIntensity*h*E;
   
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
       specular = pow(specular, 20.0f);
       
       float4 specularIntensity = tex2D(SpecularSampler,Input.mUV);
       
       specular*=specularIntensity.rgb*gLightColor;
    }
    
    float3 ambient = float3(0.1f,0.1f,0.1f)*albedo.rgb;
    
    return float4(ambient+diffuse+specular,1);
}
//--------------------------------------------------------------//
// Technique Section for NormalMapping
//--------------------------------------------------------------//
technique NormalMapping
{
   pass Pass_0
   {
      VertexShader = compile vs_1_1 NormalMapping_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 NormalMapping_Pass_0_Pixel_Shader_ps_main();
   }

}

