struct App2Vert
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;


};

float4 main(App2Vert IN) : SV_POSITION
{
	return float4(IN.Position, 1);

}