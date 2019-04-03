#include <list>
#include <vector>
#include "Error.h"
#include "dx12/DX12ResourceBindings.h"

//Static-------------------


/*
//Public----------------

FDX12ResourceBindings::FDX12ResourceBindings(const std::vector<FRB_LayoutNode> &vLayouts)
{
	//1. split nodes by resource types (Constants | CBV | SRV/UAV | Samplers)

	//2. further sort nodes by access frequency from most frequent to least
	//https://docs.microsoft.com/en-us/windows/desktop/direct3d12/creating-a-root-signature#code-for-defining-a-version-11-root-signature


	std::list<const FRB_LayoutNode *>	lpLayoutNodesConstants{},
										lpLayoutNodesInlineDescr{},
										vpLayoutNodesSamplers{};

	std::list<const FRB_LayoutNode *> lpRootConstants{}, lpRootDescriptors{}, lpTableContent{};

	//32bit word
	unsigned short RemainingSizeInWords{ 64 };

	for (auto &&Node : vLayouts)
	{		
		switch (Node.Type)
		{
		case ERB_ResourceType::Constant:
			lpLayoutNodesConstants.emplace_back(&Node);
			break;
		case ERB_ResourceType::CBV:
		case ERB_ResourceType::SRV:
		case ERB_ResourceType::UAV:
			lpLayoutNodesInlineDescr.emplace_back(&Node);
			break;
		case ERB_ResourceType::Sampler:
			vpLayoutNodesSamplers.emplace_back(&Node);
			break;
		}

	}

	for (auto &&Node : lpLayoutNodesConstants)
	{
		lpRootConstants.push_back(std::move(Node));
		RemainingSizeInWords -= Node->DataSizeInBytes / 4;
		

	}

	for (auto &&Node : lpLayoutNodesInlineDescr)
	{
		if (Node->FrequencyDataAccess == ERB_AccessFrequency::Rarely)
		{
			lpTableContent.push_back(Node);
		}
		else
		{
			lpRootDescriptors.push_back(Node);
			RemainingSizeInWords -= 2;

		}


	}

	if (lpTableContent.size() > 0)
	{
		RemainingSizeInWords -= 1;

	}
	ARI_ASSERT(RemainingSizeInWords <= 0, "Parameters for root signature are larger than 64 words");
	
	lpRootConstants.sort(&CompareNodes);
	lpRootDescriptors.sort(&CompareNodes);
	lpTableContent.sort(&CompareNodes);


//Info generation

	unsigned int ParameterCount = static_cast<unsigned int>(lpRootConstants.size() + lpRootDescriptors.size());
	ParameterCount += lpTableContent.size() > 0 ? 1 : 0;

	std::vector<D3D12_ROOT_PARAMETER1> vParameterInfos(ParameterCount);

	unsigned int ParameterIndex{ 0 };

	for (auto &&Node : lpRootConstants)
	{
		D3D12_ROOT_CONSTANTS ConstantInfo{};
		ConstantInfo.ShaderRegister = Node->RegisterSlot;
		ConstantInfo.Num32BitValues = Node->DataSizeInBytes / 4;

		vParameterInfos.at(ParameterIndex).ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		vParameterInfos.at(ParameterIndex).ShaderVisibility = TranslateShaderVisibility(Node->StageVisibility);

		++ParameterIndex;

	}

	for (auto &&Node : lpRootDescriptors)
	{
		D3D12_ROOT_DESCRIPTOR1 DescriptorInfo{};
		DescriptorInfo.ShaderRegister = Node->RegisterSlot;
		//Doc reads like Descriptor_Flags and Descriptor_Range_Flags are interchangeable:
		//https://docs.microsoft.com/en-us/windows/desktop/api/d3d12/ne-d3d12-d3d12_root_descriptor_flags#remarks
		DescriptorInfo.Flags = static_cast<D3D12_ROOT_DESCRIPTOR_FLAGS>( TranslateDescriptorFlags(Node->FrequencyChangeDescriptor, Node->FrequencyChangeData) );
		
		++ParameterIndex;
	}

	D3D12_ROOT_DESCRIPTOR_TABLE1 TableInfo{};
	auto vTableRanges = std::vector<D3D12_DESCRIPTOR_RANGE1>( lpTableContent.size() );
	if (lpTableContent.size() > 0)
	{
		unsigned int RangeIndex{ 0 };
		for (auto &&Node : lpTableContent)
		{
			vTableRanges.at(RangeIndex).BaseShaderRegister = Node->RegisterSlot;
			vTableRanges.at(RangeIndex).Flags = TranslateDescriptorFlags( Node->FrequencyChangeDescriptor, Node->FrequencyChangeData );
			vTableRanges.at(RangeIndex).NumDescriptors = 1;
			vTableRanges.at(RangeIndex).OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			vTableRanges.at(RangeIndex).RangeType = TranslateDescriptorRangeType(Node->Type);

			++RangeIndex;
		}
		TableInfo.NumDescriptorRanges = static_cast<UINT>( lpTableContent.size() );
		TableInfo.pDescriptorRanges = vTableRanges.data();
			   
		vParameterInfos.at(ParameterIndex).DescriptorTable = TableInfo;
		vParameterInfos.at(ParameterIndex).ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		
		//vParameterInfos.at(ParameterIndex).ShaderVisibility = TranslateShaderVisibility();
		++ParameterIndex;

	}


	


#pragma region TABLE
	D3D12_DESCRIPTOR_RANGE1 DescrRange[2]{};
	DescrRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//param type is srv
	DescrRange[0].NumDescriptors = 3;//3 srvs | -1 == unbounded (only for last table entry)
	DescrRange[0].BaseShaderRegister = 0; //register where the first element of the range is placed, so t0-t2
	DescrRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //places this range immediately after the last, unit is 
																							//'in descriptors from start of the root signature'

	//Flags:
	//Volatile == descriptor table data can be modified any time except during execution of that cmd list (Default root signature v1.0)
	//Default(Static) == descriptors are assumed static: descriptor table is fully initialized when set on cmd list (record)
	//		  and 'cannot be changed untile the list executes the last time' (invalidates the list ?) -- (Default for v.1.1 signatures)
		//for bundles descriptor have to be ready at record. tables to static descriptor have to be set in the bundle, they do not inherit
		//and they can not change until the bundle executes the last time (a cmd list can use the tables's descriptors when the bundle returns)
		//FOR STATIC ranges to buffer views (but not texture views) out of bounds access on hardware is undefined and can require device reset
	//Data_volatile == same a volatile but for descriptor data
	//data static while set at exec ==
		//data cant be changed starting from record until any dispatches do not reference the data
		//changing the data inside a command list, when draws are finished referencing, requires a descriptor table to be rebound to the list
		//-> this disables additional driver state tracking, bc data is guaranteed to not change without a rebound
	//data static == same as static
	//samplers do not support data flags, descriptor_volatile cannot be set with data_static, (but all others);
	DescrRange[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;

	//DescrRange[0].RegisterSpace  allow multiple descriptor arrays of unknown size to not appear to overlap, so a 5 here could map to register(t3,space5)
	DescrRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	DescrRange[1].BaseShaderRegister = 2;
	DescrRange[1].NumDescriptors = 1;
	DescrRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	DescrRange[1].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;

	D3D12_ROOT_DESCRIPTOR_TABLE1 DescrTableDesc{};
	DescrTableDesc.NumDescriptorRanges = 2;
	DescrTableDesc.pDescriptorRanges = DescrRange;
#pragma endregion


	D3D12_ROOT_PARAMETER1 RootParams[2]{};
	RootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_SRV;
	RootParams[0].DescriptorTable = DescrTableDesc;
	RootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_CONSTANTS ConstDesc{};
	ConstDesc.Num32BitValues = 2;
	ConstDesc.ShaderRegister = 0;
	

	RootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParams[1].Constants = ConstDesc;
	
	D3D12_STATIC_SAMPLER_DESC SamplerDesc{};
	SamplerDesc.ShaderRegister = 0;
	SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//...

	D3D12_ROOT_SIGNATURE_DESC1 RootDesc{};
	RootDesc.NumParameters = 2;
	RootDesc.pParameters = RootParams;
	RootDesc.NumStaticSamplers = 1;
	RootDesc.pStaticSamplers = &SamplerDesc;

	
}


//Protected---------------

bool FDX12ResourceBindings::CompareNodes(const FRB_LayoutNode *pLhs, const FRB_LayoutNode *pRhs)
{
	float RatingRhs{ 0 };
	RateNodeFrequency(pRhs, RatingRhs);
	
	float RatingLhs{ 0 };
	RateNodeFrequency(pLhs, RatingLhs);

	return (RatingLhs > RatingRhs || pLhs->Priority > pRhs->Priority);

}

void FDX12ResourceBindings::RateNodeFrequency(const FRB_LayoutNode *pNode, float &out_Rating)
{
	switch (pNode->FrequencyNode)
	{
	case ERB_NodeFrequency::Draw:
		out_Rating += 2;
		break;
	case ERB_NodeFrequency::Batch:
		out_Rating += 1;
		break;

	}


}

void FDX12ResourceBindings::RateDataFrequency(const FRB_LayoutNode *pNode, float &out_Rating)
{
	/*
	switch (pNode->FrequencyData)
	{
	case ERB_Frequency::Volatile:
		out_Rating += 2;
		break;
	case ERB_Frequency::Frame:
		out_Rating += 1;
		break;

	}
	(asterisk)/

}

D3D12_SHADER_VISIBILITY FDX12ResourceBindings::TranslateShaderVisibility(const EFShaderStages Stages) const noexcept
{
	D3D12_SHADER_VISIBILITY ShaderVisibility{};
	switch (Stages)
	{
	case EFShaderStages::VS:
		ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		break;
	case EFShaderStages::PS:
		ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		break;
	case EFShaderStages::GS:
		ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
		break;
	case EFShaderStages::TEV:
		ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
		break;
	case EFShaderStages::TCTRL:
		ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
		break;
	default:
		ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	}
	return ShaderVisibility;


}

D3D12_DESCRIPTOR_RANGE_FLAGS FDX12ResourceBindings::TranslateDescriptorFlags(EFRB_ChangeFrequency DescriptorChange, EFRB_ChangeFrequency DataChange) const noexcept
{	
	//Default is volatile for data and descriptor
	if (DescriptorChange == EFRB_ChangeFrequency::Default && DataChange == EFRB_ChangeFrequency::Default)
	{
		return D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;

	}
	D3D12_DESCRIPTOR_RANGE_FLAGS Flags{};

	//Descriptor flags
	if( (DescriptorChange & EFRB_ChangeFrequency::Static) == EFRB_ChangeFrequency::Static )
	{
		Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
	
	}
	else
	{
		Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;

	}
	
	//Data flags
	if( (DataChange & EFRB_ChangeFrequency::Static) == EFRB_ChangeFrequency::Static )
	{
		Flags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;

	}
	else if( (DataChange & EFRB_ChangeFrequency::Static_At_Execute) == EFRB_ChangeFrequency::Static_At_Execute )
	{
		Flags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;

	}
	else
	{
		Flags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;

	}
	
	return Flags;


}

D3D12_DESCRIPTOR_RANGE_TYPE FDX12ResourceBindings::TranslateDescriptorRangeType(ERB_ResourceType Type) const
{
	switch (Type)
	{
	case ERB_ResourceType::CBV:
		return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	case ERB_ResourceType::SRV:
		return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	case ERB_ResourceType::UAV:
		return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	default:
		ARI_THROW_ERROR(-1, "Translation of type not available");
		
	}
	

}
*/