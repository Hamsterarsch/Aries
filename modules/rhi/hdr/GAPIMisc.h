#pragma once

#include <cstdint>
#include "Error.h"

namespace GAPIMisc
{
	void Align(intptr_t &out_Intptr, unsigned int Alignment)
	{
		if(Alignment == 0 || (Alignment & (Alignment-1)) ) 
		{
			ARI_THROW_ERROR(-1, "Non power of to alignment");

		}
		out_Intptr = (out_Intptr + (Alignment-1) ) & ~(Alignment-1);


	}

	void Align(size_t &out_Aligned, unsigned int Alignment)
	{
		if (Alignment == 0 || (Alignment & (Alignment - 1)))
		{
			ARI_THROW_ERROR(-1, "Non power of to alignment");

		}
		out_Aligned = (out_Aligned + (Alignment - 1)) & ~(Alignment - 1);


	}


}
