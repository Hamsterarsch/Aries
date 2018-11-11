#pragma once
#include <DirectXMath.h>

__declspec(align(16)) struct FLight
{
	DirectX::XMMATRIX ToWorldMatrix;
	DirectX::XMFLOAT3 Color;
	float Range;
	float InnerAngle;
	float OuterAngle;
	float Intensity;


};
