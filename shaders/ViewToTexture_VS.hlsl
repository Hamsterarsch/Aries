cbuffer MatrixBuffer
{
	matrix World;
	matrix View;
	matrix Projection;

};

cbuffer ReflectionBuffer : register(b1)
{
	matrix ReflectionView;

};

struct App2Vert
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;


};

struct Vert2Pixel
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float3 WSPosition : TEXCOORD2;

};

Vert2Pixel main(App2Vert IN)
{
	Vert2Pixel OUT;
	OUT.Position = mul(mul(mul(Projection, ReflectionView), World), float4(IN.Position, 1));
	OUT.Uv = IN.Uv;
	OUT.Normal = mul(World, float4(IN.Normal, 1)).xyz;
	OUT.WSPosition = mul(World, float4(IN.Position, 1)).xyz;

	return OUT;


}