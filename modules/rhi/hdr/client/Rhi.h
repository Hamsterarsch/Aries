#pragma once
#include "IGAPIFactory.h"
#include "LinkageMacro.h"

RHI_API std::shared_ptr<IGAPIFactory> GetGAPIFactory(EAPIType Type);