struct VertToPixel
{
	float4 Position : SV_POSITION;
	float4 DepthPosition : TEXCOORD0;

};

struct Pixel2App
{
	float4 Color : SV_TARGET;
	//float4 Depth : SV_DEPTH;


};

Pixel2App main(VertToPixel IN)
{
	Pixel2App OUT;
	
	OUT.Color = float4(1, 1, 1, 1);
	//OUT.Depth.x = IN.DepthPosition.z / IN.DepthPosition.w;
	return OUT;



}