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
	float4 Position : POSITION;
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
	OUT.Position = mul(mul(mul(IN.Position, WorldMatrix), ViewMatrix), ProjectionMatrix);
	OUT.Uv = IN.Uv;

	matrix ReflectProjectWorldMatrix = mul(mul(WorldMatrix, ReflectionMatrix), ProjectionMatrix);
	//ReflectProjectWorldMatrix = mul(ReflectionMatrix, ProjectionMatrix);
	//ReflectProjectWorldMatrix = mul(WorldMatrix, ReflectProjectWorldMatrix);//why this order

	OUT.ReflectionPos = mul(IN.Position, ReflectProjectWorldMatrix);

	return OUT;


}