#pragma once
#include "IGAPIFactory.h"
#include "LinkageMacro.h"

#include "dx12/DX12Factory.h"

RHI_API std::shared_ptr<IGAPIFactory> GetGAPIFactory(EAPITypes Type);

RHI_API std::shared_ptr<FDX12Factory> GetDX12Factory();