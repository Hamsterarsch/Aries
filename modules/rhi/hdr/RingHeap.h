#pragma once
#include "client/IGAPIFactory.h"
#include "client/IFence.h"



#include <vector>

class FRingHeap
{
public:
	FRingHeap(IGAPIFactory &Factory, const std::vector<std::shared_ptr<IFence>> &vInflightFences, EHeapTypes Type, size_t SizeInBytes, EResourceCategory TargetCategory, bool bHasMSAAAlignment) :
		m_vInflightOffsets(vInflightFences.size())
	{
		m_pHeap = Factory.MakeHeap(Type, SizeInBytes, TargetCategory, bHasMSAAAlignment);

		for (size_t InflightIndex{ 0 }; InflightIndex < vInflightFences.size(); ++InflightIndex)
		{
			m_vInflightOffsets.at(InflightIndex).pFence = vInflightFences.at(InflightIndex);


		}
		

	}

	void Allocate(size_t InflightIndex)
	{
		auto InflightCount = m_vInflightOffsets.size();

		//if out of memory
		for (size_t OffsetIndex{ 1 }; OffsetIndex < InflightIndex - 1; ++OffsetIndex)
		{
			auto NextInMemory = (InflightIndex + OffsetIndex) % InflightCount;

			while( !m_vInflightOffsets.at(NextInMemory).pFence->IsSignaled() )
			{
				//the render thread can not proceed until the fence is signaled
				//but completion times of frames are high and a simple sleep could lead to unwanted latency.
				
				//@todo: dx example uses events but this class is api unspecific and events seem different in vulkan (not fence based)
				//so this needs refactoring

			}

		}

		//still oom

	}

protected:
	struct FInflightOffset
	{
		size_t ResourceOffset{ 0 };
		std::shared_ptr<IFence> pFence;

	};
	
	std::unique_ptr<class IHeap> m_pHeap;
	std::vector<FInflightOffset> m_vInflightOffsets;

};
