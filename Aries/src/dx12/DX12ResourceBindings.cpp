#include "dx12/DX12ResourceBindings.h"

FDX12ResourceBindings::FDX12ResourceBindings()
{
#pragma region TABLE
	D3D12_DESCRIPTOR_RANGE1 DescrRange[2]{};
	DescrRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//param type is srv
	DescrRange[0].NumDescriptors = 3;//3 srvs | -1 == unbounded (only for last table entry)
	DescrRange[0].BaseShaderRegister = 0; //register where the first element of the range is placed, so t0-t2
	DescrRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //places this range immediately after the last, unit is 
																							//'in descriptors from start of the root signature'

	//Flags:
	//Volatile == descriptor table data can be modified any time except during execution of that cmd list (Default root signature v1.0)
	//None(Static) == descriptors are assumed static: descriptor table is fully initialized when set on cmd list (record)
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