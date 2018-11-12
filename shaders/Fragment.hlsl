struct Light
{
	matrix ToWorldMatrix;
	float3 Color;
	float Range;
	float InnerAngle;
	float OuterAngle;
	float Intensity;

};

#define MaxLights 2

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
		bool bIsDirectional = bool(1 - ceil(clamp(Lights[i].Range, 0, 1)));

		float3 LightPos = Lights[i].ToWorldMatrix[3].xyz;// mul(float4(0, 0, 0, 1), Lights[i].ToWorldMatrix).xyz;

		float3 PixelToLight = LightPos - IN.WSPosition;
		float3 LightForwardVec = normalize(mul(Lights[i].ToWorldMatrix, float4(0, 0, 1, 1)).xyz - LightPos);

		float3 LightDir = normalize(lerp(PixelToLight, -LightForwardVec, bIsDirectional));

		//
		float DistToLight = length(PixelToLight);

		float RangeFalloff = lerp(1 - clamp(DistToLight / Lights[i].Range, 0, 1), 1, bIsDirectional);
		RangeFalloff *= RangeFalloff;

		//
		float AngleToLightCenter = dot(LightDir, -LightForwardVec);
		float CenterFalloff = smoothstep(cos(radians(Lights[i].OuterAngle * .5)), cos(radians(Lights[i].InnerAngle * .5)), AngleToLightCenter);
		CenterFalloff *= CenterFalloff;

		//
		float NormalAngle = clamp(dot(IN.Normal, LightDir), 0, 1);

		//spot lights currently showing unexpected behaviour (180 degree opening angle masks too early)
		float LightStrength = CenterFalloff * RangeFalloff * NormalAngle;
		LightAttenuation +=  LightStrength * Lights[i].Color * Lights[i].Intensity;
	
	}
	
	return float4((LightAttenuation + float3(.0625, .0625, .0625)) * float3(0.5, 0.5, 0.5), 1);
	

}