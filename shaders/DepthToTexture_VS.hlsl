cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

}

struct Light
{
	matrix ToWorldMatrix;
	matrix ProjectionMatrix;
	float3 Color;
	float Range;
	float InnerAngle;
	float OuterAngle;
	float Intensity;
	unsigned int LightMapStartIndex;
	unsigned int LightMapStartChannel;

};

cbuffer MatrixOverrides : register(b1)
{
	Light LightInfo;

}

struct App2Vert
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Uv : TEXCOORD0;


};

struct VertToPixel
{
	float4 Position : SV_POSITION;
	//SV_POSITION "offsets by 0.5" so we need an extra unbiased value - todo: check
	float4 DepthPosition : TEXCOORD0;

};

VertToPixel main(App2Vert IN)
{
	VertToPixel OUT;

	OUT.Position = mul(mul(mul(LightInfo.ProjectionMatrix, LightInfo.ToWorldMatrix), World), float4(IN.Position, 1));
	OUT.DepthPosition = OUT.Position;
	
	return OUT;

}
