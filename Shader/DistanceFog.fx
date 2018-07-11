//월드뷰프로젝션 행렬
float4x4 matWorld;
float4x4 matView;
float4x4 matProjection;

float4x4 lightMatrix;//라이트 매트릭스
float4 vColor; //메쉬색
float4 vFog; //(Far/(Far-Near), -1/(Far-Near)) 포그값
float4 vCameraPosition; //뷰 시점

struct VertexShaderInput
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

struct VertexShaderOutput
{
	float4 position : POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
	float fog : FOG;
};

VertexShaderOutput VertexShaderFuction(VertexShaderInput Input)
{
	VertexShaderOutput Output;

	Output.position = mul(Input.position, matWorld);
	Output.position = mul(Output.position, matView);
	Output.position = mul(Output.position, matProjection); //좌표변환

	float3 lightDir = normalize(float3(lightMatrix._21, lightMatrix._22, lightMatrix._23));

	Output.color = vColor * max(dot(lightDir, normalize(Input.normal)), 0);

	Output.tex = Input.tex;

	//거리포그
	float4 pos = Input.position - vCameraPosition;
	Output.fog = vFog.x + length(pos) * vFog.y;

	return Output;
}

technique FogShader
{
	pass Pass0
	{
		VertexShader = compile vs_2_0 VertexShaderFuction();
	}
}
