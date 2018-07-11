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
// Default_DirectX_Effect
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string Default_DirectX_Effect_Pass_0_Model : ModelData = "C:\\Program Files (x86)\\AMD\\RenderMonkey 1.82\\Examples\\Media\\Models\\Sphere.3ds";

struct VS_INPUT
{
   float4 mPosition : POSITION;
};

struct VS_OUTPUT
{
   float4 mPosition : POSITION;
   float4 vTex0 : TEXCOORD0;
   float2 vTex1 : TEXCOORD1;
   float2 vTex2 : TEXCOORD2;
};


float4x4 WorldViewProjectionMatrix : WorldViewProjection;
float4 UVData
<
   string UIName = "UVData";
   string UIWidget = "Direction";
   bool UIVisible =  false;
   float4 UIMin = float4( -10.00, -10.00, -10.00, -10.00 );
   float4 UIMax = float4( 10.00, 10.00, 10.00, 10.00 );
   bool Normalize =  false;
> = float4( 1.00, 1.00, 0.00, 0.00 );

VS_OUTPUT Default_DirectX_Effect_Pass_0_Vertex_Shader_vs_main(VS_INPUT Input)
{
   VS_OUTPUT Output;
   
   float4 Pos = mul(Input.mPosition, WorldViewProjectionMatrix);
  
   Output.mPosition = Pos.xyww;
   Output.vTex0.xyz = normalize(Input.mPosition.yzx);
   Output.vTex0.w = 1.0f;
   
   Output.vTex1 = Input.mPosition.xy+UVData.zw;
   Output.vTex2 = (Input.mPosition.xy+UVData.xy)*3.7f;
  
   return Output;
}




struct PS_INPUT
{
   float4 vTex0 : TEXCOORD0;
   float2 vTex1 : TEXCOORD1;
   float2 vTex2 : TEXCOORD2;
};

texture EnvironmentMap_Tex
<
   string ResourceName = "..\\..\\ได\?\5674-192\\bin\\media\\textures\\sky_box1.dds";
>;
samplerCUBE EnvironmentSampler = sampler_state
{
   Texture = (EnvironmentMap_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
   ADDRESSU = CLAMP;
   ADDRESSW = CLAMP;
   ADDRESSV = CLAMP;
};
texture CloudMap_Tex
<
   string ResourceName = "..\\..\\ได\?\5674-192\\bin\\media\\textures\\clouds.dds";
>;
sampler2D CloudSampler = sampler_state
{
   Texture = (CloudMap_Tex);
   MAGFILTER = LINEAR;
   MIPFILTER = LINEAR;
   MINFILTER = LINEAR;
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   ADDRESSW = WRAP;
};

float4 Default_DirectX_Effect_Pass_0_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR
{
   float4 background = texCUBE(EnvironmentSampler, normalize(Input.vTex0));
   float4 cloud0 = tex2D(CloudSampler, Input.vTex1);
   float4 cloud1 = tex2D(CloudSampler, Input.vTex2);
   
   float4 cloudLayer = cloud0*cloud1.a * background.a;
   
   return background+cloudLayer;
}


//--------------------------------------------------------------//
// Technique Section for Default_DirectX_Effect
//--------------------------------------------------------------//
technique Default_DirectX_Effect
{
   pass Pass_0
   {
      CULLMODE = NONE;
      ZENABLE = TRUE;
      ZWRITEENABLE = TRUE;
      ZFUNC = ALWAYS;
      STENCILENABLE = TRUE;
      STENCILFUNC = ALWAYS;
      STENCILPASS = REPLACE;
      STENCILREF = 0x0;
      ALPHABLENDENABLE = FALSE;
      ALPHATESTENABLE = FALSE;

      VertexShader = compile vs_2_0 Default_DirectX_Effect_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 Default_DirectX_Effect_Pass_0_Pixel_Shader_ps_main();
   }

}

