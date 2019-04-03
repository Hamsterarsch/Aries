#pragma once
#include "dx12/DX12DescriptorTable.h"


struct FDescriptorTableEntryInfo
{
	unsigned int StartShaderRegister;
	EFDescriptorFlags DescriptorRangeFlags;
	//unsigned int RangeSize;//uint max for unbounded


};

struct FDescriptorTableInfo
{
public:
	std::weak_ptr<FDX12DescriptorTable> pTable;
	std::vector<EDescriptorTypes> vSimpleEntrySpec;
	std::vector<FDescriptorTableEntryInfo> vEntryInfos;
	EShaderStages ShaderVisibility;
	

};

struct FSamplerStaticDesc : public FSamplerDesc
{
public:
	EShaderStages ShaderVisibility;
	unsigned int  ShaderRegister;


};

struct FRootSignatureInfo
{
public:
	std::vector<FDescriptorTableInfo> vTableInfos;
	std::vector<FSamplerStaticDesc> vStaticSamplers;

};


class FDX12RootSignature
{
public:
	FDX12RootSignature(FDX12Factory &Factory, const FRootSignatureInfo &Info) :
		m_pErrorBlob{ nullptr },
		m_bIsValid{ true }
	{

		std::vector<D3D12_ROOT_PARAMETER1> vParameters{};
		std::vector<std::vector<D3D12_DESCRIPTOR_RANGE1>> vvRanges{};
				
		//per table root parameters
		for(auto &&TableInfo : Info.vTableInfos)
		{			
			if(auto pTableRef{ TableInfo.pTable.lock() })
			{
				vvRanges.emplace_back(pTableRef->GetPartialRangeInfos());
				auto &vPartialRanges{ vvRanges.back() };
		
				if(vPartialRanges.size() != TableInfo.vEntryInfos.size())
				{
					//todo add log
					m_bIsValid = false;
					return;
				}
		
				unsigned int RangeIndex{ 0 };
				for(auto &&Range : vPartialRanges)
				{
					Range.BaseShaderRegister = TableInfo.vEntryInfos.at(RangeIndex).StartShaderRegister;
					Range.Flags = DX12TranslateDescriptorFlags(TableInfo.vEntryInfos.at(RangeIndex).DescriptorRangeFlags);
					++RangeIndex;
					
				}			
			}
			else if(!TableInfo.vSimpleEntrySpec.empty())
			{
				if (TableInfo.vSimpleEntrySpec.size() != TableInfo.vEntryInfos.size())
				{
					//todo add log
					m_bIsValid = false;
					return;
				}

				vvRanges.emplace_back();
				unsigned int RangeIndex{ 0 };
				for(auto &&DescriptorType : TableInfo.vSimpleEntrySpec)
				{
					vvRanges.back().emplace_back();
					auto &Range{ vvRanges.back().back() };

					Range.NumDescriptors = 1;
					Range.RangeType = DX12TranslateDescriptorTypes(DescriptorType);
					Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
					Range.BaseShaderRegister = TableInfo.vEntryInfos.at(RangeIndex).StartShaderRegister;
					Range.Flags = DX12TranslateDescriptorFlags(TableInfo.vEntryInfos.at(RangeIndex).DescriptorRangeFlags);
					++RangeIndex;

				}
			}
			else
			{
				//todo: add log
				m_bIsValid = false;
				return;				
			}

			vParameters.emplace_back();
			auto &Parameter{ vParameters.back() };

			Parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			Parameter.DescriptorTable.NumDescriptorRanges = static_cast<unsigned int>(TableInfo.vEntryInfos.size());
			Parameter.DescriptorTable.pDescriptorRanges = vvRanges.back().data();
			Parameter.ShaderVisibility = DX12TranslateStagesToVisibility(TableInfo.ShaderVisibility);
						
		}

		//static samplers
		std::vector<D3D12_STATIC_SAMPLER_DESC> vStaticSamplers{};
		for(auto &&SamplerDesc : Info.vStaticSamplers)
		{
			D3D12_STATIC_SAMPLER_DESC Desc{};
			Desc.AddressU = DX12TranslateAddressMode(SamplerDesc.U);
			Desc.AddressV = DX12TranslateAddressMode(SamplerDesc.V);
			Desc.AddressW = DX12TranslateAddressMode(SamplerDesc.W);
			Desc.Filter = DX12TranslateFilterMode(SamplerDesc.FilterMode);
			Desc.MaxAnisotropy = SamplerDesc.MaxAnisotropyLevel;
			//Integer spec only
			Desc.MinLOD = static_cast<float>(SamplerDesc.LowestMipToUse);
			Desc.MaxLOD = static_cast<float>(SamplerDesc.HighestMiptoUse);
			Desc.MipLODBias = static_cast<float>(SamplerDesc.MipBias);
			//Border always black
			//Comparision func pending
			//Desc.ComparisonFunc = DX12TranslateComparisonFunc(Info.Sampler.)
			Desc.ShaderVisibility = DX12TranslateStagesToVisibility(SamplerDesc.ShaderVisibility);
			Desc.ShaderRegister = SamplerDesc.ShaderRegister;

			vStaticSamplers.push_back(std::move(Desc));

		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC SignatureDesc{};
		SignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		
		//use input assembly by default
		SignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		SignatureDesc.Desc_1_1.NumParameters = static_cast<unsigned int>(vParameters.size());
		SignatureDesc.Desc_1_1.pParameters = vParameters.data();
		SignatureDesc.Desc_1_1.NumStaticSamplers = static_cast<unsigned int>(vStaticSamplers.size());
		SignatureDesc.Desc_1_1.pStaticSamplers = vStaticSamplers.data();
		
		auto Hr{ D3D12SerializeVersionedRootSignature(&SignatureDesc, &m_pSerializedBlob, &m_pErrorBlob) };

		if(FAILED(Hr) || m_pErrorBlob)
		{
			m_bIsValid = false;
			return;
		}

		Hr = Factory.GetDevice()->CreateRootSignature(ARI_DEFAULT_NODE_MASK, m_pSerializedBlob->GetBufferPointer(), m_pSerializedBlob->GetBufferSize(), IID_PPV_ARGS(m_pRootSignature.GetAddressOf()) );
		if(FAILED(Hr))
		{
			m_bIsValid = false;
			return;
		}


	}


	bool IsValid() const noexcept { return m_bIsValid; }
	
	ComPtr<ID3D12RootSignature> GetAPIHandle() const noexcept { return m_pRootSignature; }


private:
	ID3DBlob *m_pSerializedBlob,
			 *m_pErrorBlob;

	ComPtr<ID3D12RootSignature> m_pRootSignature;

	bool m_bIsValid;


};
