float4x4 matWorld : World;
float4x4 matViewProjection : ViewProjection;

float camNear;			//ī�޶� �ٰŸ� ���
float camFar;			//ī�޶� ���Ÿ� ���
float4 vFog;
float4 vFogColor;

//-----------------------------------------------------------------------------
// Base
//-----------------------------------------------------------------------------

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float3 Normal : NORMAL0;
   float3 BiNormal : BINORMAL0;
   float3 Tangent : TANGENT0;
   float2 BaseUV : TEXCOORD0;
   float2 TileUV : TEXCOORD1;
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float2 TileUV : TEXCOORD0;
   float2 ControlUV : TEXCOORD1;
   float3 Normal : TEXCOORD2;
   float4 FinalPos : TEXCOORD3;
   float Fog : FOG;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output = (VS_OUTPUT)0;

  

   float4 worldPos = mul( Input.Position, matWorld );
   Output.Position = mul(worldPos, matViewProjection );
  
   Output.TileUV = Input.TileUV;
   Output.ControlUV = Input.BaseUV;
     
   Output.Normal = mul( Input.Normal, (float3x3)matWorld );
  
   Output.FinalPos = Output.Position;

   Output.Fog = vFog.x + Output.Position.z*vFog.y;  //��������

	if (Output.Fog < 0.0f)
		Output.Fog = 0.f;
	else if (Output.Fog > 1.f)
		Output.Fog = 1.0f;

   return( Output );
}



struct PS_INPUT 
{
   float2 TileUV : TEXCOORD0;
   float2 ControlUV : TEXCOORD1;
   float3 Normal : TEXCOORD2;
   float4 FinalPos : TEXCOORD3;
   float Fog : FOG;
};

//�ȼ����̴� ��� ����ü
struct PS_OUTPUT
{
	float4 baseColor : COLOR0;			//0�� �������� �÷�
	float4 normalDepth : COLOR1;		//1�� �������� �÷� ( RGB �븻, A ���� )
};


texture Terrain0_Tex;
sampler2D Terrain0 = sampler_state
{
   Texture = (Terrain0_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};
texture Terrain1_Tex;
sampler2D Terrain1 = sampler_state
{
   Texture = (Terrain1_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};
texture Terrain2_Tex;
sampler2D Terrain2 = sampler_state
{
   Texture = (Terrain2_Tex);
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
   MAGFILTER = LINEAR;
};
texture Terrain3_Tex;
sampler2D Terrain3 = sampler_state
{
   Texture = (Terrain3_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
   MAXANISOTROPY = 0;
};

texture TerrainControl_Tex;
sampler2D TerrainControl = sampler_state
{
   Texture = (TerrainControl_Tex);
   MAGFILTER = ANISOTROPIC;
   MAXANISOTROPY = 16;
   MINFILTER = ANISOTROPIC;
   MIPFILTER = ANISOTROPIC;
};



float4 worldLightDir;
float4 worldLightColor;


PS_OUTPUT ps_main( PS_INPUT Input )
{   
	PS_OUTPUT  Output = (PS_OUTPUT)0;

   // Terrain Tile �÷��� ��´�.
   float3 terrain0 = tex2D( Terrain0, Input.TileUV ).rgb;
   float3 terrain1 = tex2D( Terrain1, Input.TileUV ).rgb;
   float3 terrain2 = tex2D( Terrain2, Input.TileUV ).rgb;
   float3 terrain3 = tex2D( Terrain3, Input.TileUV ).rgb;
  
   //Control Texture �� �÷��� ��´�.
   float3 controlColor = tex2D( TerrainControl, Input.ControlUV ).rgb;
   

   //��Ʈ�� Texture ���� ��������� ��´�.
   float redFactor = controlColor.r;
   float greenFactor = controlColor.g;
   float blueFactor = controlColor.b;
   float blackFactor = saturate( 1.0f - ( redFactor +  greenFactor + blueFactor ) );

   float3 texColor1 = terrain0 * redFactor;
   float3 texColor2 = terrain1 * greenFactor;
   float3 texColor3 = terrain2 * blueFactor;
   float3 texColor4 = terrain3 * blackFactor;
   
   float3 finalColor = texColor1 + texColor2 + texColor3 + texColor4;

   float3 worldNormal = normalize(Input.Normal);
   float3 dir = normalize(worldLightDir.xyz);
   float3 lightDir = -dir;
   float diff = dot(worldNormal, lightDir);
 
   diff = saturate( diff );
   
   diff = max(0.2f, diff);		//Ambient �� 0.2f



   //��ĺ�ȯ�� ��ģ �� z �� ��ĺ�ȯ���� ��� ����ġ w �� ������ 0 ~ 1 ������ ���� ���� �ȴ�.
   float depth = Input.FinalPos.z / Input.FinalPos.w;

   //���� depth ���� ī�޶��� near �� far �� �̿��Ͽ� �������� ���ش�....
   //Perspective Projection Linear Depth
   float z = depth;
   float a = camFar / (camFar - camNear);
   float b = -camNear / (camFar - camNear);
   depth = b / (z - a);

   float4 Color = float4(finalColor * diff, 1);

   Output.baseColor = Input.Fog*Color + (1 - Input.Fog)*vFogColor;
   Output.normalDepth = float4(worldNormal, depth);		//alpha ���� ������ ���.

   return Output;
}



//---------------------------------------------------------------
// CreateShadow ����
//---------------------------------------------------------------

struct VS_INPUT_DEPTH
{
   float4 Position : POSITION0;
};

struct VS_OUTPUT_DEPTH
{
   float4 Position : POSITION0;
   float4 Depth : TEXCOORD0;
};


VS_OUTPUT_DEPTH vs_depth( VS_INPUT_DEPTH Input )
{
   VS_OUTPUT_DEPTH Output = (VS_OUTPUT_DEPTH)0;

   float4 worldPos = mul( Input.Position, matWorld );
   Output.Position = mul( worldPos, matViewProjection );
   
   Output.Depth = Output.Position;

   return( Output );
}

float4 ps_CreateShadow( VS_OUTPUT_DEPTH Input ) : COLOR0
{   
	//��ĺ�ȯ�� ��ģ �� z �� ��ĺ�ȯ���� ��� ����ġ w �� ������ 0 ~ 1 ������ ���� ���� �ȴ�.
	float depth = Input.Depth.z / Input.Depth.w;

    //return float4( depth, depth, depth, 1 );

	return float4( depth.xxx, 1 );
}



//--------------------------------------------------------------//
// Render With ShadowMap 
//--------------------------------------------------------------//

float4x4 matLightViewProjection;			//���⼺ ���� ViewProjection ���

//������ Texture
texture Shadow_Tex;
sampler2D ShadowSampler = sampler_state
{
   texture = <Shadow_Tex>;
   ADDRESSU = BORDER;
   ADDRESSV = BORDER;
   BORDERCOLOR = 0xFFFFFFFF;
};

float bias = 0.01f;


struct VS_INPUT_RECIVESHADOW
{
   float4 Position : POSITION0;
   float3 Normal : NORMAL0;
   float3 Binormal : BINORMAL0;
   float3 Tangent : TANGENT0;
   float2 BaseUV : TEXCOORD0;
   float2 TileUV : TEXCOORD1;
};

struct VS_OUTPUT_RECIVESHADOW
{
   float4 Position : POSITION0;
   float2 TileUV : TEXCOORD0;
   float2 ControlUV : TEXCOORD1;
   float3 Normal : TEXCOORD2;
   float4 FinalPos : TEXCOORD3;
   float4 LightClipPos : TEXCOORD4;		//���� ���忡�� �ٶ� ��ġ
   float Fog : FOG;
};

VS_OUTPUT_RECIVESHADOW vs_ReciveShadow( VS_INPUT_RECIVESHADOW Input )
{
   VS_OUTPUT_RECIVESHADOW Output = (VS_OUTPUT_RECIVESHADOW)0;

   float4 worldPos = mul( Input.Position, matWorld );
   Output.Position = mul( worldPos, matViewProjection );
  
   Output.TileUV = Input.TileUV;
   Output.ControlUV = Input.BaseUV;
     
   Output.Normal = mul( Input.Normal, (float3x3)matWorld );

   Output.FinalPos = Output.Position;
   Output.LightClipPos = mul( worldPos, matLightViewProjection );


   Output.Fog = vFog.x + Output.Position.z*vFog.y; //��������

   if (Output.Fog < 0.f)
	   Output.Fog = 0.f;
   else if (Output.Fog > 1.f)
	   Output.Fog = 1.f;

  
   return( Output );
}



PS_OUTPUT ps_ReciveShadow( VS_OUTPUT_RECIVESHADOW Input )
{   
	PS_OUTPUT Output = (PS_OUTPUT)0;


	//���� ���忡�� �ٶ� ��ġ�� ���� �� ( ����Ʈ ����� �����̱⶧���� �������� ���ɴ� )
	float lightDepth = Input.LightClipPos.z / Input.LightClipPos.w;

	//Shadow ���� UV ����
	//( -1 ~ 1 )
	float2 shadowUV = Input.LightClipPos.xy / Input.LightClipPos.w;
	shadowUV.y = -shadowUV.y;		//y����...
	//0 ~ 1 �� ����
	shadowUV = ( shadowUV * 0.5f ) + 0.5f;	

	//���� �׷����� ���� ��ġ�� DirectionLight ���忡�� ���� ���̰�...
	float shadowDepth = tex2D( ShadowSampler, shadowUV ).r;

	//�׸��ڰ� �׷����� ��Ȳ�� shadowDepth + bias �� ���� lightDepth �� ū����̴�.

   // Terrain Tile �÷��� ��´�.
   float3 terrain0 = tex2D( Terrain0, Input.TileUV ).rgb;
   float3 terrain1 = tex2D( Terrain1, Input.TileUV ).rgb;
   float3 terrain2 = tex2D( Terrain2, Input.TileUV ).rgb;
   float3 terrain3 = tex2D( Terrain3, Input.TileUV ).rgb;
  
   //Control Texture �� �÷��� ��´�.
   float3 controlColor = tex2D( TerrainControl, Input.ControlUV ).rgb;
   

   //��Ʈ�� Texture ���� ��������� ��´�.
   float redFactor = controlColor.r;
   float greenFactor = controlColor.g;
   float blueFactor = controlColor.b;
   float blackFactor = saturate( 1.0f - ( redFactor +  greenFactor + blueFactor ) );

   float3 texColor1 = terrain0 * redFactor;
   float3 texColor2 = terrain1 * greenFactor;
   float3 texColor3 = terrain2 * blueFactor;
   float3 texColor4 = terrain3 * blackFactor;
   
   float3 finalColor = texColor1 + texColor2 + texColor3 + texColor4;
   float3 worldNormal = normalize(Input.Normal);
   float3 dir = normalize(worldLightDir.xyz);
   float3 lightDir = -dir;
   float diff = dot(worldNormal, lightDir);

   //�׸��ڰ� �׷����� �Ѵٸ� diff �� ������ ������ �ȴ� 
   if (shadowDepth + bias < lightDepth && lightDepth < 1.0f)
   {
	   diff = abs(diff) * -1.0f;
   }

   
	diff = max(0.2f, diff);		//Ambient �� 0.2f
 
   

   //��ĺ�ȯ�� ��ģ �� z �� ��ĺ�ȯ���� ��� ����ġ w �� ������ 0 ~ 1 ������ ���� ���� �ȴ�.
   float depth = Input.FinalPos.z / Input.FinalPos.w;

   //���� depth ���� ī�޶��� near �� far �� �̿��Ͽ� �������� ���ش�....
   //Perspective Projection Linear Depth
   float z = depth;
   float a = camFar / (camFar - camNear);
   float b = -camNear / (camFar - camNear);
   depth = b / (z - a);


   float4 Color = float4(finalColor * diff, 1);

   Output.baseColor = Input.Fog* Color + (1 - Input.Fog)*vFogColor;
   Output.normalDepth = float4(worldNormal, depth);		//alpha ���� ������ ���.

   return Output;
}




technique Base
{
   pass Pass_0
   {
      VertexShader = compile vs_3_0 vs_main();
      PixelShader = compile ps_3_0 ps_main();
   }
}


technique CreateShadow
{
   pass Pass_0
   {
      VertexShader = compile vs_3_0 vs_depth();
      PixelShader = compile ps_3_0 ps_CreateShadow();
   }
}



technique ReciveShadow
{
   pass Pass_0
   {
      VertexShader = compile vs_3_0 vs_ReciveShadow();
      PixelShader = compile ps_3_0 ps_ReciveShadow();
   }
}
