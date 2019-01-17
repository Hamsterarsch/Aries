#pragma once
#include "client/GAPITypes.h"

enum class EBufferTypes : char
{
	Constant,
	Vertex,
	Index


};

enum class EAPITypes : char;

class IReservedBuffer
{
public:
	virtual ~IReservedBuffer() = default;

	virtual EAPITypes GetAPIType() const noexcept = 0;


};
