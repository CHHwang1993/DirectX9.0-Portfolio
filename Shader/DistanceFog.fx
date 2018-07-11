//������������� ���
float4x4 matWorld;
float4x4 matView;
float4x4 matProjection;

float4x4 lightMatrix;//����Ʈ ��Ʈ����
float4 vColor; //�޽���
float4 vFog; //(Far/(Far-Near), -1/(Far-Near)) ���װ�
float4 vCameraPosition; //�� ����

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
	Output.position = mul(Output.position, matProjection); //��ǥ��ȯ

	float3 lightDir = normalize(float3(lightMatrix._21, lightMatrix._22, lightMatrix._23));

	Output.color = vColor * max(dot(lightDir, normalize(Input.normal)), 0);

	Output.tex = Input.tex;

	//�Ÿ�����
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
