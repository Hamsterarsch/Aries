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
	
	OUT.WSPosition = mul(float4(IN.Position, 1), World);
	OUT.Position = mul(mul(float4(OUT.WSPosition, 1), View), Projection);
	OUT.Normal = normalize( mul(IN.Normal, World) );
	OUT.UV = IN.UV;

	return OUT;

}
