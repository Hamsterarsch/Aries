SamplerState SampleType;
Texture2D ReflectionTexture;

struct VertToPixel
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD0;
	float4 ReflectionPos : TEXCOORD1;


};

float4 main(VertToPixel IN) : SV_TARGET
{
	float2 ReflectUv;
	ReflectUv.x = (IN.ReflectionPos.x / IN.ReflectionPos.w) / 2.f + .5f;
	ReflectUv.y = (-IN.ReflectionPos.y / IN.ReflectionPos.w) / 2.f + .5f;//y is inverted because the geometry depicted by the reflection is 'upside down'

	float4 ReflectionColor = ReflectionTexture.Sample(SampleType, ReflectUv) * float4( .33, .33, .33, 1);

	return ReflectionColor;


}