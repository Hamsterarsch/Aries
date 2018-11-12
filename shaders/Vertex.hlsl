cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

};

struct App2Vert
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 UV : TEXCOORD0;

};

struct Vert2Frag
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float3 WSPosition : TEXCOORD2;

};

Vert2Frag main(App2Vert IN)
{
	Vert2Frag OUT;
	
	OUT.WSPosition = mul(World, float4(IN.Position, 1));
	OUT.Position = mul(mul(Projection, View), float4(OUT.WSPosition, 1));
	OUT.Normal = normalize( mul(World, IN.Normal) );
	OUT.UV = IN.UV;

	return OUT;

}
