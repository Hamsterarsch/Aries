cbuffer MatrixBuffer
{
	matrix World;
	matrix View;
	matrix Projection;


};

cbuffer MappedMatrices : register(b1)
{
	matrix MappedView;
	matrix MappedProjection;


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
	float3 Normal : NORMAL;
	float3 LightPos : TEXCOORD1;
	float4 LightViewPos : TEXCOORD2;

};

Vert2Pixel main(App2Vert IN)
{
	Vert2Pixel OUT;

	OUT.Uv = IN.Uv;

	OUT.Position = mul(mul(mul(Projection, View), World), float4(IN.Position, 1));
	OUT.LightViewPos = mul(mul(mul(MappedProjection, MappedView), World), float4(IN.Position, 1));

	OUT.Normal = normalize(mul(World, float4(IN.Normal, 1)).xyz);

	OUT.LightPos = MappedView[3].xyz;

	return OUT;


}