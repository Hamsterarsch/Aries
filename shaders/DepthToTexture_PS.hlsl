struct VertToPixel
{
	float4 Position : SV_POSITION;
	float4 DepthPosition : TEXCOORD0;


};

struct Pixel2App
{
	float4 FragColor : SV_TARGET;


};

Pixel2App main(VertToPixel IN)
{
	Pixel2App OUT;
	
	OUT.FragColor = float4(0, 0, 0, 0);
	OUT.FragColor.x = IN.DepthPosition.z / IN.DepthPosition.w;
	return OUT;
	

}