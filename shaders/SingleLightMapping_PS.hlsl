struct Vert2Pixel
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 LightPos : TEXCOORD1;
	float4 LightViewPos : TEXCOORD2;

};

Texture2D DepthTexture;
SamplerState SampleClamped;

float4 main(Vert2Pixel IN) : SV_TARGET
{
	float2 ProjectedUv = float2(IN.LightViewPos.xy);
	ProjectedUv = ProjectedUv / IN.LightViewPos.w / 2 + .5f;
	ProjectedUv.y *= -1;
	ProjectedUv.y += 1;

	bool bInViewRange = (bool)(saturate(ProjectedUv) == ProjectedUv);

	//
	float OccludersDepth = DepthTexture.Sample(SampleClamped, ProjectedUv).r;
	float FragmentDepthFromMappedView = (IN.LightViewPos.z / IN.LightViewPos.w) -0.001f;
	
	bool bIsUnshadowed = FragmentDepthFromMappedView < OccludersDepth;

	float4 FragColor;

	float3 Ambient = float3(.125f, .125f, .125f);

	FragColor = float4(Ambient + float3(.5, .5, .5) * bIsUnshadowed * bInViewRange, 1) * float4(.5, .5, .5, 1);
	//FragColor = float4(bInViewRange, bIsUnshadowed , 0, 1);

	return FragColor;
	

}