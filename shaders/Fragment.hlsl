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

#define MaxLights 1

cbuffer LightBuffer : register(b0)
{
	Light Lights[MaxLights];
	
};

struct Vert2Frag
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float3 WSPosition : TEXCOORD2;

};

float4 main(Vert2Frag IN) : SV_TARGET
{
	float3 LightAttenuation = float3(0,0,0);

	for (int i = 0; i < MaxLights; ++i)
	{
		//Directional part
		bool bIsDirectional = bool(1 - ceil(saturate(Lights[i].Range)) );

		float3 LightPos = Lights[i].ToWorldMatrix._m03_m13_m23;//todo [3].xyz; has a different result
		float3 PixelToLight = LightPos - IN.WSPosition;
		float3 LightForwardVec = normalize(Lights[i].ToWorldMatrix._m02_m12_m22);//todo [2].xyz has the same result

		float3 LightDir = normalize(lerp(PixelToLight, -LightForwardVec, bIsDirectional));

		//length from pixel to light
		float DistToLight = length(PixelToLight);

		//range from light center (1) to border(0) or constantly 1 for directional lights (for that the term has no influence)
		float RangeFalloff = lerp(1 - saturate(DistToLight / Lights[i].Range), 1, bIsDirectional);
		//squared falloff
		RangeFalloff *= RangeFalloff;

		//Angle between the center of the light and the dir from pixel to light. 
		//(inverse the forward vec bc the light dir is not from the light but from the pixel)
		float AngleToLightCenter = dot(LightDir, -LightForwardVec);

		//Make a falloff based on the ligthDir angle from the outer angle (0) to the inner angle (1)
		float CenterFalloff = smoothstep(cos(radians(Lights[i].OuterAngle * .5)), cos(radians(Lights[i].InnerAngle * .5)), AngleToLightCenter);
		//squared falloff
		CenterFalloff *= CenterFalloff;
		
		//normal influence, clamped to 0-1
		float NormalAngle = saturate(dot(IN.Normal, LightDir));

		//spot lights currently showing unexpected behaviour (180 degree opening angle masks too early)
		float LightStrength = CenterFalloff * RangeFalloff * NormalAngle;
		//return float4(CenterFalloff, RangeFalloff, NormalAngle, 1);

		LightAttenuation +=  LightStrength * Lights[i].Color * Lights[i].Intensity;

	
	}
	
	return float4((LightAttenuation + float3(.0625, .0625, .0625)) * float3(0.5, 0.5, 0.5), 1);
	

}