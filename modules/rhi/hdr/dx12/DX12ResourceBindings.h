#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <vector>
#include "EnumFlags.h"
#include "IResourceBindings.h"



using Microsoft::WRL::ComPtr;



enum class ERB_ResourceType : unsigned char
{
	Unspecified,
	SRV,
	CBV,
	UAV,
	Sampler,
	Constant

};

enum class ERB_NodeFrequency : unsigned char
{
	Draw,
	Batch

};

enum class ERB_AccessFrequency : unsigned char
{
	Frequently,
	Rarely

};

enum class EFRB_ChangeFrequency : unsigned char
{
	Default,
	Static				= 1 << 0,
	Volatile			= 1 << 1,
	Static_At_Execute	= 1 << 2

};
GENERATE_BINARY_OPERATORS_FOR_ENUM_FLAGS(EFRB_ChangeFrequency);

enum class EFShaderStages : unsigned char
{
	VS		= 1 << 0,
	PS		= 1 << 1,
	GS		= 1 << 2,
	TEV		= 1 << 3,
	TCTRL	= 1 << 4,
	ALL     = static_cast<unsigned char>(~0)

};
GENERATE_BINARY_OPERATORS_FOR_ENUM_FLAGS(EFShaderStages);

struct FRB_LayoutNode
{
public:
	ERB_ResourceType Type;
	unsigned int RegisterSlot;
	ERB_NodeFrequency FrequencyNode;
	ERB_AccessFrequency FrequencyDataAccess;
	EFRB_ChangeFrequency FrequencyChangeDescriptor;
	EFRB_ChangeFrequency FrequencyChangeData;
	EFShaderStages StageVisibility;
	unsigned int DataSizeInBytes;
	float Priority;


};

class FDX12ResourceBindings : public IResourceBindings
{
public:
	FDX12ResourceBindings(const std::vector<FRB_LayoutNode> &vLayouts);

protected:
	static bool CompareNodes(const FRB_LayoutNode *pLhs, const FRB_LayoutNode *pRhs);
	static void RateNodeFrequency(const FRB_LayoutNode *pNode, float &out_Rating);
	static void RateDataFrequency(const FRB_LayoutNode *pNode, float &out_Rating);
	
	D3D12_SHADER_VISIBILITY TranslateShaderVisibility(EFShaderStages Stages) const noexcept;
	D3D12_DESCRIPTOR_RANGE_FLAGS TranslateDescriptorFlags(EFRB_ChangeFrequency DescriptorChange, EFRB_ChangeFrequency DataChange) const noexcept;
	D3D12_DESCRIPTOR_RANGE_TYPE TranslateDescriptorRangeType(ERB_ResourceType Type) const;

	ComPtr<ID3D12RootSignature> m_pRootSignature;



};


