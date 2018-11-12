cbuffer MatrixBuffer
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjectionMatrix;


};

cbuffer ReflectionBuffer : register(b1)
{
	matrix ReflectionMatrix;
	

};

struct AppToVert
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;


};

struct VertToPixel
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD0;
	float4 ReflectionPos : TEXCOORD1;


};

VertToPixel main(AppToVert IN)
{
	VertToPixel OUT;
	
	//IN.Position.w = 1;
	
	OUT.Position = mul(mul(mul(ProjectionMatrix, ViewMatrix), WorldMatrix), float4(IN.Position, 1));//mul(mul(mul(float4(IN.Position, 1), WorldMatrix), ViewMatrix), ProjectionMatrix);
	OUT.Uv = IN.Uv;

	matrix WorldReflectProject = mul(mul(ProjectionMatrix, ReflectionMatrix), WorldMatrix);

	OUT.ReflectionPos = mul(WorldReflectProject, float4(IN.Position, 1));

	return OUT;


}