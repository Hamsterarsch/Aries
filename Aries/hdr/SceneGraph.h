#pragma once
#include "Light.h"
#include "IActor.h"
#include <vector>
#include <memory>

class FSceneGraph
{
public:
	std::vector<FLight> m_vLights;
	std::vector<std::unique_ptr<IActor>> m_vActors;


};
