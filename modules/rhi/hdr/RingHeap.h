#pragma once
#include "client/IGAPIFactory.h"





#include <queue>
class FRingHeap
{
public:
	FRingHeap(IGAPIFactory &Factory, EHeapType Type, size_t SizeInBytes, EResourceCategory TargetCategory, bool bHasMSAAAlignment)
	{
		m_pHeap = Factory.MakeHeap(Type, SizeInBytes, TargetCategory, bHasMSAAAlignment);
		

	}

	

protected:
	struct FInflightOffset
	{
		size_t FrameIndex;
		size_t ResourceOffset;

	};
	
	std::unique_ptr<class IHeap> m_pHeap;
	std::queue<FInflightOffset> m_InflightQueue;

};
